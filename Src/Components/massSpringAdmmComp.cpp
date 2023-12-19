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

	this->dt  /= this->numADMMIterations;

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
	energy -= 0.5 * this->positions.transpose() * this->J * this->D;
	energy += this->positions.transpose() * this->force;
}

void MassSpringADMM::update(const EngineState& engineState)
{
	if(engineState.keyboard->released.contains(GLFW_KEY_SPACE))
		this->isPinVertex = false;
}

void MassSpringADMM::fixedUpdate(const EngineState& engineState)
{
	CustomUtils::Stopwatch sw("MassSpringADMM::fixedUpdate");
	this->integrate();
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
	this->inertia = this->positions;
	this->optimizeD();
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

	if (this->isPinVertex)
		force.segment<3>(this->pinnedVertex * 3).setZero();
}

void MassSpringADMM::integrate()
{
	double prevEnergy = this->calculateEnergy();
	double energy;
	for (size_t i = 0; i < this->numADMMIterations; i++)
	{
		this->inertia = this->positions + this->dt * this->velocity;
		if(this->isPinVertex)
			this->inertia.segment<3>(this->pinnedVertex * 3) = this->positions.segment<3>(this->pinnedVertex * 3);
		this->optimizeD();
		this->optimizeX();
		energy = this->calculateEnergy();
		double diff = std::abs(prevEnergy - energy);
		if (diff < 1e-6)
		{
			std::cout << "\nConverged in " << i << " iterations" << std::endl;
		}
		if(i==this->numADMMIterations - 1)
			std::cout << "Energy resisual at end of step:" << diff << std::endl;
		prevEnergy = energy;
	}
	this->velocity *= 0.99;
	this->tetMesh->isDirty = true;

}

void MassSpringADMM::optimizeD()
{
	// iterate through all springs and compute D
	#pragma omp parallel for
	for (int i = 0; i < springs.size(); i++)
	{
		Eigen::Vector3d springVector = this->inertia.segment<3>(3 * springs[i].first) - this->inertia.segment<3>(3 * springs[i].second);
		double springLength = springVector.norm();
		assert(springLength > 0.0);

		// Compute spring force
		this->D.segment<3>(3 * i) = this->restLengths[i] * springVector.normalized();
	}
}

void MassSpringADMM::optimizeX()
{

	this->b = this->dt * this->dt * this->J * this->D + this->massMatrix * this->inertia + this->force * this->dt * this->dt;
	this->oldPos = Eigen::VectorXd(this->positions);
	this->positions = this->lltSolver.solve(this->b);
	if(this->isPinVertex)
		this->positions.segment<3>(this->pinnedVertex * 3) = this->oldPos.segment<3>(this->pinnedVertex * 3);
	this->velocity = (this->positions - this->oldPos) / this->dt;
	if(this->isPinVertex)
		this->velocity.segment<3>(this->pinnedVertex * 3).setZero();
	this->handleCollisions();
}


void MassSpringADMM::handleCollisions()
{
	// Check for collisions with ground
	#pragma omp parallel for
	for (int i = 0; i < positions.size() / 3; i++)
	{
		if (positions(3 * i + 1) <= 0.0)
		{
			positions(3 * i + 1) = std::abs(positions(3 * i + 1)) * 0.1;
			velocity.segment<3>(3*i) *= -0.1;
		}
	}
}

double MassSpringADMM::calculateEnergy()
{
	Eigen::VectorXd x_y = velocity * dt;
	double energy = 0.5 * x_y.transpose() * this->massMatrix * x_y;
	for (size_t i = 0; i < springs.size(); i++)
	{
		Eigen::Vector3d springVector = this->positions.segment<3>(3 * springs[i].first) - this->positions.segment<3>(3 * springs[i].second);
		double springLength = springVector.norm();
		assert(springLength > 0.0);
		energy += 0.5 * this->springStiffness * (springLength - this->restLengths[i]) * (springLength - this->restLengths[i]);
	}
	return energy;
}
