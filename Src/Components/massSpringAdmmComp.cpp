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
		restLengths.emplace_back((tetMesh->tetData.vertices.segment<3>(3 * curSpr.first) - tetMesh->tetData.vertices.segment<3>(3 * curSpr.second)).norm());
		auto test = (tetMesh->tetData.vertices.segment<3>(3 * curSpr.first) - tetMesh->tetData.vertices.segment<3>(3 * curSpr.second)).norm();
		assert(restLengths.back() > 0.0);
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
	this->weightedLaplacianTerm.resize(this->positions.size(), this->positions.size());
	this->weightedLaplacianTerm.reserve(Eigen::VectorXi::Constant(this->positions.size(), this->tetMesh->tetData.maxVertexNeighbors));
	double multiplier = this->springStiffness * this->dt * this->dt;
	for (auto& vertAdj : this->tetMesh->tetData.vertAdjacency)
	{
		for (auto& adjVert : vertAdj.second)
		{
			this->weightedLaplacianTerm.insert(vertAdj.first, adjVert) = -multiplier;
			this->weightedLaplacianTerm.insert(vertAdj.first + 1, adjVert + 1) = -multiplier;
			this->weightedLaplacianTerm.insert(vertAdj.first + 2, adjVert + 2) = -multiplier;
		}
		this->weightedLaplacianTerm.insert(vertAdj.first, vertAdj.first) = (double)vertAdj.second.size() * multiplier;
		this->weightedLaplacianTerm.insert(vertAdj.first + 1, vertAdj.first + 1) = (double)vertAdj.second.size() * multiplier;
		this->weightedLaplacianTerm.insert(vertAdj.first + 2, vertAdj.first + 2) = (double)vertAdj.second.size() * multiplier;
	}
	this->weightedLaplacianTerm.makeCompressed();
	this->weightedLaplacianTerm = this->massMatrix +  this->weightedLaplacianTerm;
	this->weightedLaplacianTerm.makeCompressed();
	this->lltSolver.compute(this->weightedLaplacianTerm);

	// Compute J
	this->J.resize(this->positions.size(), 3 * this->springs.size());
	this->J.reserve(Eigen::VectorXi::Constant(this->positions.size(), 2));
	for (int i = 0; i < this->springs.size(); i++)
	{
		this->J.insert(this->springs[i].first, 3 * i) = multiplier;
		this->J.insert(this->springs[i].first + 1, 3 * i + 1) = multiplier;
		this->J.insert(this->springs[i].first + 2, 3 * i + 2) = multiplier;

		this->J.insert(this->springs[i].second, 3 * i) = -multiplier;
		this->J.insert(this->springs[i].second + 1, 3 * i + 1) = -multiplier;
		this->J.insert(this->springs[i].second + 2, 3 * i + 2) = -multiplier;
	}
	this->J.makeCompressed();
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
	this->b = this->J * this->D + this->massMatrix * this->inertia + this-> force * this->dt * this->dt;
	auto oldPos = this->positions;
	this->positions = this->lltSolver.solve(this->b);
	this->velocity = (this->positions - this->oldPos) / this->dt;
	this->velocity *= 0.99;
	this->tetMesh->isDirty = true;

}

void MassSpringADMM::computeD()
{
	// iterate through all springs and compute D
	#pragma omp parallel for
	for (int i = 0; i < springs.size(); i++)
	{
		Eigen::Vector3d springVector = positions.segment<3>(3 * springs[i].second) - positions.segment<3>(3 * springs[i].first);
		double springLength = springVector.norm();
		assert(springLength > 0.0);

		// Compute spring force
		this->D.segment<3>(3 * i) = this->restLengths[i] * springVector.normalized();
	}
}

void MassSpringADMM::handleCollisions()
{
}