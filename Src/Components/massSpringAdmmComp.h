#pragma once

#include <Eigen/Sparse>
#include "includes.h"
#include "Component.h"
#include "tetMesh.h"

class MassSpringADMM : public Component
{
public:
	MassSpringADMM(std::shared_ptr<TetMesh> _tetMesh);
	virtual void Start() override;
	virtual void update(const EngineState& engineState) override;
	virtual void fixedUpdate(const EngineState& engineState) override;

	void preComputeMatrices();
	void calculateForces();
	void integrate();
	void optimizeD();
	void optimizeX();
	void computeLaplacianTerm();
	void handleCollisions();

private:
	std::shared_ptr<TetMesh> tetMesh;

	// Physical parameters
	std::vector<std::pair<uint32_t, uint32_t>> springs;
	std::vector<double> restLengths;

	Eigen::Vector3d gravity = Eigen::Vector3d(0.0, -9.8, 0.0);
	double dt = 1.0 / 60.0;
	double springStiffness = 300000;
	double damping = 0.0f;
	double perVertexMass = 1;
	double collisionPenalty = 5000000.0;
	int pinnedVertex = 100;
	bool isPinVertex = true;

	Eigen::VectorXd force, velocity, inertia,D,b,oldPos;
	Eigen::VectorXd& positions;

	Eigen::SparseMatrix<double> massMatrix, weightedLaplacianTerm, J;

	Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> lltSolver;
	int numADMMIterations = 20;
};