#pragma once

#include <Eigen/Sparse>
#include "includes.h"
#include "component.h"
#include "tetMesh.h"

class MassSpring : public Component
{
public:
	MassSpring(std::shared_ptr<TetMesh> _tetMesh);
	void start(const EngineState& engineState) override;
	void update(const EngineState& engineState) override;
	void fixedUpdate(const EngineState& engineState) override;

	void calculateMassMatrix();
	void calculateForces();
	void handleCollisions();
	void initJacobian();
	void calculateJacobian();
	void addMatrixToJacobian(int row, int col, double* values, const Eigen::Matrix3d& mat,const double& mul,const int& nnz,  bool isAtomic);
	void integrate();

	~MassSpring();

private:
	std::shared_ptr<TetMesh> tetMesh; 

	// Physical parameters
	std::vector<std::pair<uint32_t, uint32_t>> springs;
	std::vector<double> restLengths;

	Eigen::Vector3d gravity = Eigen::Vector3d(0.0, -9.8, 0.0);
	double dt;
	double springStiffness = 80000;
	double damping = 0.0f;
	double perVertexMass = 1.0;
	int pinnedVertex = 100;
	bool isPinVertex = true;
	bool cgSolver = false;
	bool isSimulate = false;

	Eigen::VectorXd force,velocity;
	Eigen::VectorXd& positions;
	
	Eigen::SparseMatrix<double> massMatrix,jacobian;

	std::unordered_map<uint32_t, int> matrixToValuesMap;

	Eigen::ConjugateGradient<Eigen::SparseMatrix<double>> solver;
	Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> ldlt_solver;
};