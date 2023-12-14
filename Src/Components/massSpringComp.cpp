#include "massSpringComp.h"

MassSpring::MassSpring(std::shared_ptr<TetMesh> _tetMesh)
{
	tetMesh = _tetMesh;
}

void MassSpring::Start()
{
	std::cout << "\nMassSpring Start" << std::endl;
	// Find all unique springs
	
	std::unordered_set<std::pair<uint32_t, uint32_t>, CustomUtils::PairHash> uniqueSprings;
	std::pair<uint32_t,uint32_t> currSpring;
	for (uint32_t i = 0; i < tetMesh->tetData.tetrahedra.rows(); i++)
	{
		for (uint32_t j = 0; j < 4; j++)
		{
			for (uint32_t k = j + 1; k < 4; k++)
			{
				currSpring.first = tetMesh->tetData.tetrahedra(i,j);
				currSpring.second = tetMesh->tetData.tetrahedra(i,k);
				if (currSpring.first > currSpring.second)
					std::swap(currSpring.first, currSpring.second);
				uniqueSprings.insert(currSpring);
			}
		}
	}
	std::cout<<"\nNumber of unique springs: "<<uniqueSprings.size()<<std::endl;

	// Compute rest lengths
	for (std::pair<uint32_t, uint32_t> curSpr : uniqueSprings)
	{
		springs.emplace_back(curSpr);
		restLengths.emplace_back((tetMesh->tetData.vertices.segment<3>(3 * curSpr.first) - tetMesh->tetData.vertices.segment<3>(3* curSpr.second)).norm());
		auto test = (tetMesh->tetData.vertices.segment<3>(3 * curSpr.first) - tetMesh->tetData.vertices.segment<3>(3 * curSpr.second)).norm();
		assert(restLengths.back() > 0.0);
	}

	positions = tetMesh->tetData.vertices;
	velocity.resizeLike(positions);
	velocity.setZero();
	force.resizeLike(positions);


	calculateMassMatrix();
	
	initJacobian();
}

void MassSpring::fixedUpdate(float dt)
{
	this->calculateForces();
	this->handleCollisions();
	this->integrate(dt,false);
}

void MassSpring::calculateForces()
{
	// Iterate through all vertices and apply gravity
	#pragma omp parallel for
	for (int i = 0; i < positions.size() / 3; i++)
	{		
		this->force.segment<3>(3 * i) = this->gravity;
	}

	force = this->massMatrix * force;

	// Iterate through all springs and apply spring forces
	#pragma omp parallel for
	for (int i = 0; i < springs.size(); i++)
	{
		Eigen::Vector3f springForces = Eigen::Vector3f::Zero();
		Eigen::Vector3f springVector = positions.segment<3>(3 * springs[i].second) - positions.segment<3>(3 * springs[i].first);
		float springLength = springVector.norm();
		assert(springLength > 0.0);
		
		// Compute spring force
		springForces = (this->springStiffness) * (springVector - restLengths[i] * springVector.normalized()) / restLengths[i];

		// Compute damping force
		//Eigen::Vector3f velocityDifference = this->velocity.segment<3>(3 * springs[i].second) - this->velocity.segment<3>(3 * springs[i].first);
		//springForces += this->damping *  velocityDifference.dot(springVector) * springVector;

		for (int j = 0; j < 3; j++)
		{
			#pragma omp atomic
			this->force(3 * springs[i].first + j) += springForces(j);
			#pragma omp atomic
			this->force(3 * springs[i].second + j) -= springForces(j);
		}		
	}
	if(this->isPinFirstVertex)
		force.segment<3>(0).setZero();
}

void MassSpring::handleCollisions()
{

}

void MassSpring::initJacobian()
{
	CustomUtils::Stopwatch sw("initJacobian");
	this->jacobian.resize(this->positions.size(), this->positions.size());
	this->jacobian.reserve(Eigen::VectorXi::Constant(this->positions.size(), 30));
	std::unordered_set<int> visitedVerts;
	std::unordered_map<int,int> springIndexMap;

	for (int i = 0; i < springs.size(); i++)
	{
		bool isFirstVertVisited = visitedVerts.contains(springs[i].first);
		bool isSecondVertVisited = visitedVerts.contains(springs[i].second);
		// insert matrix into jacobian
		for (int j = 0; j < 3; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				if(!isFirstVertVisited)
				this->jacobian.insert(3 * springs[i].first + j, 3 * springs[i].first + k) = (3 * springs[i].first + j) + positions.size() * (3 * springs[i].first + k);
				if(!isSecondVertVisited)
				this->jacobian.insert(3 * springs[i].second + j, 3 * springs[i].second + k) = (3 * springs[i].second + j) + positions.size() * (3 * springs[i].second + k);
				this->jacobian.insert(3 * springs[i].first + j, 3 * springs[i].second + k) = (3 * springs[i].first + j) + positions.size() * (3 * springs[i].second + k);
				this->jacobian.insert(3 * springs[i].second + j, 3 * springs[i].first + k) = (3 * springs[i].second + j) + positions.size() * (3 * springs[i].first + k);
			}
		}
		visitedVerts.insert(springs[i].first);
		visitedVerts.insert(springs[i].second);
	}

	jacobian.makeCompressed();
	auto values = jacobian.valuePtr();
	int k = 0;
	for (int i = 0; i < jacobian.outerSize(); i++)
	{
		for (Eigen::SparseMatrix<float>::InnerIterator it(jacobian, i); it; ++it)
		{
			this->matrixToValuesMap[std::make_pair(it.row(), it.col())] = k++;
		}
	
	}
}

void MassSpring::calculateJacobian()
{
	CustomUtils::Stopwatch sw("calculateJacobian");
	float* values = jacobian.valuePtr();
	for (size_t i = 0; i < jacobian.nonZeros(); i++)	
		values[i] = 0.0f;
	
	

	// Iterate through all springs and caculate jacobian
	#pragma omp parallel for
	for (int i = 0; i < springs.size(); i++)
	{
		Eigen::Vector3f springVector = positions.segment<3>(3 * springs[i].second) - positions.segment<3>(3 * springs[i].first);
		float springLength = springVector.norm();
		assert(springLength > 0.0);
		// Compute spring force
		Eigen::Matrix3f Kii = springVector * springVector.transpose();
		float l2 = springVector.squaredNorm();
		float l = sqrt(l2);
		Eigen::Matrix3f term1 = Eigen::Matrix3f::Identity() - Kii / l2;
		Eigen::Matrix3f K_spring = this->springStiffness * (-Eigen::Matrix3f::Identity() + (this->restLengths[i] / l) * term1) / this->restLengths[i];
		if (l < restLengths[i])
		{
			K_spring = -springVector.normalized() * springVector.transpose() * this->springStiffness / this->restLengths[i];
		}
		Kii = K_spring;
		float firstMultiplier = (springs[i].first != 0 || !isPinFirstVertex) ? 1.0f : 0.0f;
		float secondMultiplier = (springs[i].second != 0 || !isPinFirstVertex) ? 1.0f : 0.0f;

		// insert matrix into jacobian
		for (int j = 0; j < 3; j++)
		{
			for (int k = 0; k < 3; k++)
			{
		
				
					#pragma omp atomic
					values[matrixToValuesMap[std::make_pair(3 * springs[i].first + j, 3 * springs[i].first + k)]] += Kii(j, k) * firstMultiplier;
					values[matrixToValuesMap[std::make_pair(3 * springs[i].first + j, 3 * springs[i].second + k)]] = -Kii(j, k) * firstMultiplier;
				
				
					#pragma omp atomic
					values[matrixToValuesMap[std::make_pair(3 * springs[i].second + j, 3 * springs[i].second + k)]] += Kii(j, k) * secondMultiplier;
					values[matrixToValuesMap[std::make_pair(3 * springs[i].second + j, 3 * springs[i].first + k)]] = -Kii(j, k) * secondMultiplier;

				
			}
		}
	}



}

void MassSpring::integrate(float dt, bool cgSolver)
{
	this->calculateJacobian();
	Eigen::SparseMatrix<double> A = (this->massMatrix - dt * dt * this->jacobian).cast<double>();	
	Eigen::VectorXd b = (this->massMatrix * this->velocity + dt * this->force).cast<double>();
	if (this->isPinFirstVertex)
	{
		b.segment<3>(0).setZero();
	}
	if (cgSolver)
	{
		solver.compute(A);
		this->velocity = solver.solveWithGuess(b, this->velocity.cast<double>()).cast<float>();
	}
	else
	{
		ldlt_solver.compute(A);
		this->velocity = ldlt_solver.solve(b).cast<float>();		
	}
	if(this->isPinFirstVertex)
		this->velocity.segment<3>(0).setZero();
	this->positions += dt * this->velocity;
	this->tetMesh->tetData.vertices = this->positions;
	this->tetMesh->isDirty = true;
}


// iterate through each tetrahedron and assign weights to each vertex based on the tetrahedron volume
void MassSpring::calculateMassMatrix()
{
	CustomUtils::Stopwatch sw("calculateMassMatrix");

	this->massMatrix.resize(positions.size(), positions.size());
	this->massMatrix.reserve(Eigen::VectorXi::Constant(positions.size(), 1));

	//// find total volume of the mesh
	//float totalVolume = 0.0f;
	//std::vector<float> tetVolumes(tetMesh->tetData.tetrahedra.rows());
	//Eigen::VectorXf massDiagonal(this->positions.size() / 3);
	//massDiagonal.setZero();
	//#pragma omp parallel for
	//for (int i = 0; i < tetMesh->tetData.tetrahedra.rows(); i++)
	//{
	//	Eigen::Vector3f v0, v1, v2, v3;
	//	v0 = this->positions.segment<3>(3 * tetMesh->tetData.tetrahedra(i, 0));
	//	v1 = this->positions.segment<3>(3 * tetMesh->tetData.tetrahedra(i, 1));
	//	v2 = this->positions.segment<3>(3 * tetMesh->tetData.tetrahedra(i, 2));
	//	v3 = this->positions.segment<3>(3 * tetMesh->tetData.tetrahedra(i, 3));
	//	// use Cayley-Menger determinant to find volume
	//	Eigen::Matrix<float,5,5> cayleyMenger;
	//	cayleyMenger << 0, 1, 1, 1, 1,
	//		1, 0, (v0 - v1).squaredNorm(), (v0 - v2).squaredNorm(), (v0 - v3).squaredNorm(),
	//		1, (v0 - v1).squaredNorm(), 0, (v1 - v2).squaredNorm(), (v1 - v3).squaredNorm(),
	//		1, (v0 - v2).squaredNorm(), (v1 - v2).squaredNorm(), 0, (v2 - v3).squaredNorm(),
	//		1, (v0 - v3).squaredNorm(), (v1 - v3).squaredNorm(), (v2 - v3).squaredNorm(), 0;
	//	float volume = sqrt(cayleyMenger.determinant() / 288.0);
	//	assert(volume > 0.0f && std::isnan(volume) == false);
	//	tetVolumes[i] = volume;
	//	//#pragma omp atomic
	//	totalVolume += volume;
	//}

	//// iterate through each tetrahedron and assign weights to each vertex based on the tetrahedron volume
	//#pragma omp parallel for
	//for (int i = 0; i < tetMesh->tetData.tetrahedra.rows(); i++)
	//{
	//	for (int j = 0; j < 4; j++)
	//	{
	//		//#pragma omp atomic
	//		massDiagonal(tetMesh->tetData.tetrahedra(i, j)) += this->totalMass * tetVolumes[i] / totalVolume;
	//	}
	//}

	//#pragma omp parallel for
	//for (int i = 0; i < massDiagonal.size(); i++)	
	//	massDiagonal(i) /= 4;

	//assert(CustomUtils::epsEqual(massDiagonal.sum(), this->totalMass, 3.0f));
	
	//#pragma omp parallel for
	/*for (int i = 0; i < massDiagonal.size(); i++)
	{
		float mass = 1.0f;
		for (int j = 0; j < 3; j++)
		{
			this->massMatrix.insert(3 * i + j, 3 * i + j) = mass;
		}
	}*/
	// Make diagonal entries of mass matrix equal to 1
	for (int i = 0; i < positions.size(); i++)
	{
		massMatrix.insert(i, i) = 1.0f;
	}

	massMatrix.makeCompressed();
}

MassSpring::~MassSpring()
{
}