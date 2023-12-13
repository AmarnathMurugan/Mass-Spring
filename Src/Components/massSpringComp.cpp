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
	
	calculateJacobian();
}

void MassSpring::fixedUpdate(float dt)
{
	this->calculateForces();
	this->handleCollisions();
	this->integrate(dt);
}

void MassSpring::calculateForces()
{
	// Iterate through all vertices and apply gravity
	#pragma omp parallel for
	for (int i = 0; i < positions.size() / 3; i++)
	{		
		this->force.segment<3>(3 * i) = this->gravity;
	}

	// Iterate through all springs and apply spring forces
	#pragma omp parallel for
	for (int i = 0; i < springs.size(); i++)
	{
		Eigen::Vector3f springForces = Eigen::Vector3f::Zero();
		Eigen::Vector3f springVector = positions.segment<3>(3 * springs[i].second) - positions.segment<3>(3 * springs[i].first);
		float springLength = springVector.norm();
		assert(springLength > 0.0);
		springVector.normalize();
		// Compute spring force
		springForces = (this->springStiffness) * (springLength - restLengths[i]) * springVector;

		// Compute damping force
		Eigen::Vector3f velocityDifference = this->velocity.segment<3>(3 * springs[i].second) - this->velocity.segment<3>(3 * springs[i].first);
		springForces += this->damping *  velocityDifference.dot(springVector) * springVector;
		/*if (springForces.squaredNorm() > 100 * 100)
			springForces = springForces.normalized() * 100;*/

		for (int j = 0; j < 3; j++)
		{
			#pragma omp atomic
			this->force(3 * springs[i].first + j) += springForces(j);
			#pragma omp atomic
			this->force(3 * springs[i].second + j) -= springForces(j);
		}		
	}
	this->force.segment<3>(0) = Eigen::Vector3f::Zero();
}

void MassSpring::handleCollisions()
{

}

void MassSpring::calculateJacobian()
{
	bool isFirstTime = this->jacobian.size() == 0;

	if (isFirstTime)
	{
			this->jacobian.resize(this->positions.size(), this->positions.size());
			int maxValence = 0;
			for (auto& [vert, adjVerts] : this->tetMesh->tetData.vertAdjacency)
			{
				maxValence = std::max(maxValence, (int)adjVerts.size());
			}
			this->jacobian.reserve(Eigen::VectorXi::Constant(this->positions.size(),maxValence*6));
			this->jacobian.setZero();
	}
	else
	{
		this->jacobian.setZero();
	}

	// Iterate through all springs and caculate jacobian
	//#pragma omp parallel for
	for (int i = 0; i < springs.size(); i++)
	{
		Eigen::Vector3f springVector = positions.segment<3>(3 * springs[i].second) - positions.segment<3>(3 * springs[i].first);
		float springLength = springVector.norm();
		assert(springLength > 0.0);
		// Compute spring force
		Eigen::Matrix3f Kii = (this->springStiffness) * (-Eigen::Matrix3f::Identity() +
											(springLength / this->restLengths[i]) * (Eigen::Matrix3f::Identity()
											- springVector * springVector.transpose() / (springLength* springLength)));
		
		// insert matrix into jacobian
		for (int j = 0; j < 3; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				//#pragma omp atomic
				this->jacobian.coeffRef(3 * springs[i].first + j, 3 * springs[i].first + k) += Kii(j, k);
				//#pragma omp atomic
				this->jacobian.coeffRef(3 * springs[i].second + j, 3 * springs[i].second + k) += Kii(j, k);
				//#pragma omp atomic
				this->jacobian.coeffRef(3 * springs[i].first + j, 3 * springs[i].second + k) -= Kii(j, k);
				//#pragma omp atomic
				this->jacobian.coeffRef(3 * springs[i].second + j, 3 * springs[i].first + k) -= Kii(j, k);
			}
		}
	}
	if(isFirstTime)
	jacobian.makeCompressed();


}

void MassSpring::integrate(float dt)
{
	//dt = dt / 10;
	Eigen::SparseMatrix<double> A = (this->massMatrix - dt * dt * this->jacobian).cast<double>();
	Eigen::VectorXd b = (this->massMatrix * this->velocity + dt * this->force).cast<double>();
	solver.compute(A);
	this->velocity = solver.solveWithGuess(b, this->velocity.cast<double>()).cast<float>();
	//float error = (A * this->velocity - b).norm();
	this->velocity.segment<3>(0) = Eigen::Vector3f::Zero();
	this->positions += dt * this->velocity;
	this->tetMesh->tetData.vertices = this->positions;
	this->tetMesh->isDirty = true;
}


// iterate through each tetrahedron and assign weights to each vertex based on the tetrahedron volume
void MassSpring::calculateMassMatrix()
{
	CustomUtils::Stopwatch sw("calculateMassMatrix");

	this->massMatrix.resize(positions.size(), positions.size());
	this->massMatrix.setZero();
	this->massMatrix.reserve(Eigen::VectorXi::Constant(positions.size(), 1));

	// find total volume of the mesh
	float totalVolume = 0.0f;
	std::vector<float> tetVolumes(tetMesh->tetData.tetrahedra.rows());
	Eigen::VectorXf massDiagonal(this->positions.size() / 3);
	massDiagonal.setZero();
	#pragma omp parallel for
	for (int i = 0; i < tetMesh->tetData.tetrahedra.rows(); i++)
	{
		Eigen::Vector3f v0, v1, v2, v3;
		v0 = this->positions.segment<3>(3 * tetMesh->tetData.tetrahedra(i, 0));
		v1 = this->positions.segment<3>(3 * tetMesh->tetData.tetrahedra(i, 1));
		v2 = this->positions.segment<3>(3 * tetMesh->tetData.tetrahedra(i, 2));
		v3 = this->positions.segment<3>(3 * tetMesh->tetData.tetrahedra(i, 3));
		// use Cayley-Menger determinant to find volume
		Eigen::Matrix<float,5,5> cayleyMenger;
		cayleyMenger << 0, 1, 1, 1, 1,
			1, 0, (v0 - v1).squaredNorm(), (v0 - v2).squaredNorm(), (v0 - v3).squaredNorm(),
			1, (v0 - v1).squaredNorm(), 0, (v1 - v2).squaredNorm(), (v1 - v3).squaredNorm(),
			1, (v0 - v2).squaredNorm(), (v1 - v2).squaredNorm(), 0, (v2 - v3).squaredNorm(),
			1, (v0 - v3).squaredNorm(), (v1 - v3).squaredNorm(), (v2 - v3).squaredNorm(), 0;
		float volume = sqrt(cayleyMenger.determinant() / 288.0);
		assert(volume > 0.0f && std::isnan(volume) == false);
		tetVolumes[i] = volume;
		#pragma omp atomic
		totalVolume += volume;
	}

	// iterate through each tetrahedron and assign weights to each vertex based on the tetrahedron volume
	#pragma omp parallel for
	for (int i = 0; i < tetMesh->tetData.tetrahedra.rows(); i++)
	{
		for (int j = 0; j < 4; j++)
		{
			#pragma omp atomic
			massDiagonal(tetMesh->tetData.tetrahedra(i, j)) += this->totalMass * tetVolumes[i] / totalVolume;
		}
	}

	#pragma omp parallel for
	for (int i = 0; i < massDiagonal.size(); i++)	
		massDiagonal(i) /= 4;

	assert(CustomUtils::epsEqual(massDiagonal.sum(), this->totalMass, 3.0f));
	
	// fill mass inverse matrix with the diagonal entries
	#pragma omp parallel for
	for (int i = 0; i < massDiagonal.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			this->massMatrix.insert(3 * i + j, 3 * i + j) = massDiagonal(i);
		}
	}
	massMatrix.makeCompressed();
}

MassSpring::~MassSpring()
{
}