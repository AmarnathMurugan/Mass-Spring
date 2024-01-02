#pragma once

#include <Eigen/Sparse>
#include "includes.h"
#include "Component.h"
#include "tetMesh.h"

class MassSpringADMM : public component
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
	double dt = 1.0 / 30.0;
	double springStiffness = 500000;
	double perVertexMass = 1;
	int pinnedVertex = 100;
	bool isPinVertex = true;
	bool isSimulate = false;

	Eigen::VectorXd force, velocity, inertia,D,b,oldPos;
	Eigen::VectorXd& positions;

	Eigen::SparseMatrix<double> massMatrix, weightedLaplacianTerm, J;

	Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> lltSolver;
	int numADMMIterations = 20;
};