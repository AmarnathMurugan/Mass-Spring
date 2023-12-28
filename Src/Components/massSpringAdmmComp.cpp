#include "massSpringAdmmComp.h"

MassSpringADMM::MassSpringADMM(std::shared_ptr<TetMesh> _tetMesh) : positions(_tetMesh->tetData.vertices)
{
	tetMesh = _tetMesh;
}

void MassSpringADMM::Start()
{
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
	
	for (std::pair<uint32_t, uint32_t> curSpr : uniqueSprings)
	{
		springs.emplace_back(curSpr);
	}

	// Compute rest lengths
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
}

void MassSpringADMM::update(const EngineState& engineState)
{
	if (engineState.keyboard->released.contains(GLFW_KEY_SPACE))
	{
		this->isPinVertex = false;
		this->calculateForces();
	}
	if (engineState.keyboard->released.contains(GLFW_KEY_ENTER))	
		this->isSimulate = true;	
}

void MassSpringADMM::fixedUpdate(const EngineState& engineState)
{
	if (!this->isSimulate)
		return;
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
	Eigen::SparseMatrix<double> Amat = this->massMatrix + this->dt * this->dt * this->weightedLaplacianTerm;
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
	for (size_t i = 0; i < this->numADMMIterations; i++)
	{
		this->oldPos = Eigen::VectorXd(this->positions);
		this->inertia = this->positions + this->dt * this->velocity;
		if(this->isPinVertex)
			this->inertia.segment<3>(this->pinnedVertex * 3) = this->positions.segment<3>(this->pinnedVertex * 3);
		this->optimizeD();
		this->optimizeX();		
		if (this->isPinVertex)
			this->positions.segment<3>(this->pinnedVertex * 3) = this->oldPos.segment<3>(this->pinnedVertex * 3);
		this->velocity = (this->positions - this->oldPos) / this->dt;
		if (this->isPinVertex)
			this->velocity.segment<3>(this->pinnedVertex * 3).setZero();
	}
	this->handleCollisions();
	//this->velocity *= 0.99;
	this->tetMesh->isDirty = true;
}

void MassSpringADMM::optimizeD()
{
	// iterate through all springs and compute D
	#pragma omp parallel for
	for (int i = 0; i < springs.size(); i++)
	{
		Eigen::Vector3d springVector = this->positions.segment<3>(3 * springs[i].first) - this->positions.segment<3>(3 * springs[i].second);
		assert(springVector.squaredNorm() > 0.0);

		// Compute spring force
		this->D.segment<3>(3 * i) = this->restLengths[i] * springVector.normalized();
	}
}

void MassSpringADMM::optimizeX()
{
	this->b = this->dt * this->dt * this->J * this->D + this->massMatrix * this->inertia + this->force * this->dt * this->dt;	
	this->positions = this->lltSolver.solve(this->b);	
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
