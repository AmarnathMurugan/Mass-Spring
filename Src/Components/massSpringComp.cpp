#include "massSpringComp.h"

MassSpring::MassSpring(std::shared_ptr<TetMesh> _tetMesh):positions(_tetMesh->tetData.vertices)
{
	tetMesh = _tetMesh;
	//positions = tetMesh->tetData.vertices;
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

void MassSpring::fixedUpdate(float _dt)
{
	//CustomUtils::Stopwatch sw("MassSpring::fixedUpdate");
	dt = _dt;
	this->calculateForces();
	this->handleCollisions();
	this->integrate();
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
		Eigen::Vector3d springForces = Eigen::Vector3d::Zero();
		Eigen::Vector3d springVector = positions.segment<3>(3 * springs[i].second) - positions.segment<3>(3 * springs[i].first);
		double springLength = springVector.norm();
		assert(springLength > 0.0);
		
		// Compute spring force
		springForces = (this->springStiffness / restLengths[i]) * (springVector - restLengths[i] * springVector.normalized());

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
#pragma omp parallel for
	for (int i = 0; i < positions.size() / 3; i++)
	{
		if (this->positions(3 * i + 1) < 0.0)
		{
			this->force(3 * i + 1) += this->collisionPenalty * std::abs(this->positions(3 * i + 1)) * dt * dt;
		}
	}
}

void MassSpring::initJacobian()
{
	CustomUtils::Stopwatch sw("initJacobian");
	this->jacobian.resize(this->positions.size(), this->positions.size());
	this->jacobian.reserve(Eigen::VectorXi::Constant(this->positions.size(), 300));
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
	int numNonZeros = jacobian.nonZeros();
	// Iterate through all non zero elements and get flat index of the first column element of all matrices
	for (int i = 0; i < jacobian.outerSize(); i++)
	{
		for (Eigen::SparseMatrix<double>::InnerIterator it(jacobian, i); it; ++it)
		{
			int row = it.row();
			int col = it.col();
			if(row % 3 == 0)
				this->matrixToValuesMap[row*numNonZeros+col] = k;
			k++;
		}
	
	}
}

void MassSpring::calculateJacobian()
{
	//CustomUtils::Stopwatch sw("calculateJacobian");
	double* values = jacobian.valuePtr();
	int nnz = jacobian.nonZeros();
	#pragma omp parallel
	for (size_t i = 0; i < nnz; i++)	
		values[i] = 0.0f;
	
	

	// Iterate through all springs and caculate jacobian
	#pragma omp parallel for
	for (int i = 0; i < springs.size(); i++)
	{
		Eigen::Vector3d springVector = positions.segment<3>(3 * springs[i].second) - positions.segment<3>(3 * springs[i].first);
		double springLength = springVector.norm();
		assert(springLength > 0.0);
		// Compute spring force
		Eigen::Matrix3d Kii = springVector * springVector.transpose();
		double l2 = springVector.squaredNorm();
		double l = sqrt(l2);
		Eigen::Matrix3d term1 = Eigen::Matrix3d::Identity() - Kii / l2;
		Eigen::Matrix3d K_spring = this->springStiffness * (-Eigen::Matrix3d::Identity() + (this->restLengths[i] / l) * term1) / this->restLengths[i];
		if (l < restLengths[i])
		{
			K_spring = -springVector.normalized() * springVector.transpose() * this->springStiffness / this->restLengths[i];
		}
		Kii = K_spring;

		
		double firstMultiplier = (springs[i].first != 0 || !isPinFirstVertex) ? 1.0f : 0.0f;
		double secondMultiplier = (springs[i].second != 0 || !isPinFirstVertex) ? 1.0f : 0.0f;

		addMatrixToJacobian(springs[i].first, springs[i].first, values, Kii, firstMultiplier,nnz, true);
		addMatrixToJacobian(springs[i].first, springs[i].second, values, -Kii, firstMultiplier, nnz, false);
		addMatrixToJacobian(springs[i].second, springs[i].second, values, Kii, secondMultiplier, nnz, true);
		addMatrixToJacobian(springs[i].second, springs[i].first, values, -Kii, secondMultiplier, nnz, false);
	}
}

void MassSpring::addMatrixToJacobian(int row, int col, double* values, const Eigen::Matrix3d& mat, const double& mul, const int& nnz, bool isAtomic)
{
	
	for (int k = 0; k < 3; k++)
	{
		int firstIndex = matrixToValuesMap[3 * row * nnz + 3 * col + k];
		for (int j = 0; j < 3; j++)
			if (isAtomic)
			{
				#pragma omp atomic
				values[firstIndex + j] += mat(j, k) * mul;
			}
			else
			{
				values[firstIndex + j] = mat(j, k) * mul;
			}

	}
}

void MassSpring::integrate()
{	
	this->calculateJacobian();
	Eigen::SparseMatrix<double> A = (this->massMatrix - dt * dt * this->jacobian);	
	Eigen::VectorXd b = (this->massMatrix * this->velocity + dt * this->force);
	if (this->isPinFirstVertex)
	{
		b.segment<3>(0).setZero();
	}
	if (cgSolver)
	{
		CustomUtils::Stopwatch sw("cg solve");
		solver.compute(A);
		this->velocity = solver.solveWithGuess(b, this->velocity);
	}
	else
	{
		CustomUtils::Stopwatch sw("ldlt solve");
		ldlt_solver.compute(A);
		this->velocity = ldlt_solver.solve(b);		
	}
	if(this->isPinFirstVertex)
		this->velocity.segment<3>(0).setZero();
	this->positions += dt * this->velocity;
	this->tetMesh->isDirty = true;
}


// iterate through each tetrahedron and assign weights to each vertex based on the tetrahedron volume
void MassSpring::calculateMassMatrix()
{
	//CustomUtils::Stopwatch sw("calculateMassMatrix");

	this->massMatrix.resize(positions.size(), positions.size());
	this->massMatrix.reserve(Eigen::VectorXi::Constant(positions.size(), 1));
	// Make diagonal entries of mass matrix equal to 1
	for (int i = 0; i < positions.size(); i++)
	{
		massMatrix.insert(i, i) = perVertMass;
	}

	massMatrix.makeCompressed();
}

MassSpring::~MassSpring()
{
}