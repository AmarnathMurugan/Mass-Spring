#include "massSpringAdmmComp.h"

MassSpringADMM::MassSpringADMM(std::shared_ptr<TetMesh> _tetMesh) : positions(_tetMesh->tetData.vertices)
{
	tetMesh = _tetMesh;
}

void MassSpringADMM::Start()
{
	std::cout << "\nMassSpringADMM Start" << std::endl;
	// Find all unique springs

	std::unordered_set<std::pair<uint32_t, uint32_t>, CustomUtils::PairHash> uniqueSprings;
	std::pair<uint32_t, uint32_t> currSpring;
	for (uint32_t i = 0; i < tetMesh->tetData.tetrahedra.rows(); i++)
	{
		for (uint32_t j = 0; j < 4; j++)
		{
			for (uint32_t k = j + 1; k < 4; k++)
			{
				currSpring.first = tetMesh->tetData.tetrahedra(i, j);
				currSpring.second = tetMesh->tetData.tetrahedra(i, k);
				if (currSpring.first > currSpring.second)
					std::swap(currSpring.first, currSpring.second);
				uniqueSprings.insert(currSpring);
			}
		}
	}
	std::cout << "\nNumber of unique springs: " << uniqueSprings.size() << std::endl;

	// Compute rest lengths
	for (std::pair<uint32_t, uint32_t> curSpr : uniqueSprings)
	{
		springs.emplace_back(curSpr);
	}

	for (auto& spring : springs)
	{
		restLengths.push_back((positions.segment<3>(3 * spring.first) - positions.segment<3>(3 * spring.second)).norm());
	}	

	velocity.resizeLike(positions);
	velocity.setZero();
	force.resizeLike(positions);
	// Initialize D
	this->D.resize(3 * this->springs.size());

	// Initialize b
	this->b.resize(this->positions.size());

	// Initialize inertia
	this->inertia.resizeLike(this->positions);

	// Initialize oldPos
	this->oldPos.resizeLike(this->positions);
	
	this->preComputeMatrices();
	this->calculateForces();

	double energy = 0.5 * this->positions.transpose() * this->weightedLaplacianTerm * this->positions;
	energy -= this->positions.transpose() * this->J * this->D;
	energy += this->positions.transpose() * this->force;
}

void MassSpringADMM::update(const EngineState& engineState)
{
}

void MassSpringADMM::fixedUpdate(const EngineState& engineState)
{
	//CustomUtils::Stopwatch sw("MassSpringADMM::fixedUpdate");

	this->integrate();
	//this->handleCollisions();
}

void MassSpringADMM::preComputeMatrices()
{
	// Compute mass matrix
	this->massMatrix.resize(this->positions.size(), this->positions.size());
	this->massMatrix.reserve(Eigen::VectorXi::Constant(this->positions.size(), 1));
	for (int i = 0; i < this->positions.size(); i++)
	{
		this->massMatrix.insert(i, i) = this->perVertexMass;
	}
	this->massMatrix.makeCompressed();

	// Compute laplacian matrix
	this->computeLaplacianTerm();

	// Compute J
	this->J.resize(this->positions.size(), 3 * this->springs.size());
	this->J.reserve(Eigen::VectorXi::Constant(3 * this->springs.size(), 2));
	
	int springIndex = 0;
	for (auto& spring : this->springs)
	{
		for (size_t i = 0; i < 3; i++)
		{
			this->J.insert(3 * spring.first + i, 3 * springIndex+i) = this->springStiffness;
			this->J.insert(3 * spring.second + i, 3 * springIndex+i) = -this->springStiffness; 
		}
		springIndex++;
	}
	this->J.makeCompressed();

	this->computeD();
	
	Eigen::MatrixXd Jdense = Eigen::MatrixXd(this->J);
	Eigen::MatrixXd test;
	test.resize(this->positions.size()/3, this->springs.size());
	test.setZero();
	Eigen::VectorXd A,S;
	A.resize(this->positions.size() / 3);
	S.resize(this->springs.size());
	springIndex = 0;
	for (auto& spring : this->springs)
	{
		A.setZero();
		A(spring.first) = 1.0;
		A(spring.second) = -1.0;
		S.setZero();
		S(springIndex++) = 1.0;
		test += A * S.transpose();
	}
	test *= this->springStiffness;
	Eigen::MatrixXd finalMat;
	finalMat.resize(this->positions.size(), 3*this->springs.size());
	finalMat.setZero();
	for (int i = 0; i < this->positions.size() / 3; i++)
	{
		for (int j = 0; j < this->springs.size(); j++)
		{
			for (int k = 0; k < 3; k++)
			{
				finalMat(3 * i + k, 3 * j + k) = test(i, j);
			}
		}
	}
	double norm = (finalMat - Jdense).norm();
	assert(norm < 1e-10);

}


void MassSpringADMM::computeLaplacianTerm()
{
	this->weightedLaplacianTerm.resize(this->positions.size(), this->positions.size());
	this->weightedLaplacianTerm.reserve(Eigen::VectorXi::Constant(this->positions.size(), this->tetMesh->tetData.maxVertexNeighbors));
	double multiplier = this->springStiffness;
	for (auto& spring : this->springs)
	{
		for (size_t i = 0; i < 3; i++)
		{
			this->weightedLaplacianTerm.coeffRef(3 * spring.first + i, 3 * spring.first + i) += multiplier;
			this->weightedLaplacianTerm.coeffRef(3 * spring.second + i, 3 * spring.second + i) += multiplier;
			this->weightedLaplacianTerm.coeffRef(3 * spring.first + i, 3 * spring.second + i) = -multiplier;
			this->weightedLaplacianTerm.coeffRef(3 * spring.second + i, 3 * spring.first + i) = -multiplier;
		}
	}
	this->weightedLaplacianTerm.makeCompressed();
	int nnz = this->weightedLaplacianTerm.nonZeros();
	Eigen::SparseMatrix<double> Amat = this->massMatrix + this->dt * this->dt * this->weightedLaplacianTerm;
	int nnz1 = Amat.nonZeros();
	this->lltSolver.compute(Amat);

	Eigen::MatrixXd laplacianTerm = Eigen::MatrixXd(this->weightedLaplacianTerm);
	Eigen::MatrixXd test,finalMat;
	test.resize(this->positions.size()/3, this->positions.size()/3);
	test.setZero();
	Eigen::VectorXd A;
	A.resize(this->positions.size()/3);
	for (auto& spring : this->springs)
	{
		A.setZero();
		A(spring.first) = 1.0;
		A(spring.second) = -1.0;
		test += A * A.transpose();
	}
	test *= this->springStiffness;
	finalMat.resize(this->positions.size(), this->positions.size());
	finalMat.setZero();
	for (int i = 0; i < this->positions.size() / 3; i++)
	{
		for (int j = 0; j < this->positions.size() / 3; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				finalMat(3 * i + k, 3 * j + k) = test(i, j);
			}
		}
	}
	double norm = (finalMat - laplacianTerm).norm();
	assert(norm < 1e-10);
}

void MassSpringADMM::calculateForces()
{
	// Iterate through all vertices and apply gravity
	#pragma omp parallel for
	for (int i = 0; i < positions.size() / 3; i++)
	{
		this->force.segment<3>(3 * i) = this->gravity;
	}

	force = this->massMatrix * force;

	//if (this->isPinVertex)
	//	force.segment<3>(this->pinnedVertex * 3).setZero();
}

void MassSpringADMM::integrate()
{
	this->computeD();
	this->inertia = this->positions + this->dt * this->velocity;
	double testnorm = (this->inertia - this->positions).norm();
	this->b = this->dt * this->dt * this->J * this->D + this->massMatrix * this->inertia + this->force * this->dt * this->dt;
	this->oldPos = Eigen::VectorXd(this->positions);
	this->positions = this->lltSolver.solve(this->b);
	this->velocity = (this->positions - this->oldPos) / this->dt;
	double velocityNorm = this->velocity.norm();
	//this->velocity *= 0.99;
	this->tetMesh->isDirty = true;

}

void MassSpringADMM::computeD()
{
	// iterate through all springs and compute D
	D.setZero();
	//#pragma omp parallel for
	for (int i = 0; i < springs.size(); i++)
	{
		Eigen::Vector3d springVector = this->positions.segment<3>(3 * springs[i].first) - this->positions.segment<3>(3 * springs[i].second);
		double springLength = springVector.norm();
		assert(springLength > 0.0);

		// Compute spring force
		this->D.segment<3>(3 * i) = this->restLengths[i] * springVector.normalized();
	}
}


void MassSpringADMM::handleCollisions()
{
}