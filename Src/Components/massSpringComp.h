#pragma once

#include <Eigen/Sparse>
#include "includes.h"
#include "Component.h"
#include "tetMesh.h"

class MassSpring : public Component
{
public:
	MassSpring(std::shared_ptr<TetMesh> _tetMesh);
	virtual void Start() override;
	virtual void update() override {  }
	virtual void fixedUpdate(float dt) override;

	void calculateMassMatrix();
	void calculateForces();
	void handleCollisions();
	void initJacobian();
	void calculateJacobian();
	void integrate(float dt, bool cgSolver = true);

	~MassSpring();

private:
	std::shared_ptr<TetMesh> tetMesh; 

	// Physical parameters
	std::vector<std::pair<uint32_t, uint32_t>> springs;
	std::vector<float> restLengths;

	Eigen::Vector3f gravity = Eigen::Vector3f(0.0f, -9.8f, 0.0f);
	float springStiffness = 100;
	float damping = 0.0f;
	float totalMass = 30.50f;
	bool isPinFirstVertex = false;

	Eigen::VectorXf positions,force,velocity;
	
	Eigen::SparseMatrix<float> massMatrix,jacobian;

	std::unordered_map<std::pair<int,int>, int, CustomUtils::PairHash> matrixToValuesMap;

	Eigen::ConjugateGradient<Eigen::SparseMatrix<double>> solver;
	Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> ldlt_solver;
};