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
	void addValueToJacobian(int row, int col, float* values, const Eigen::Matrix3f& mat,const float& mul,const int& nnz,  bool isAtomic);
	void integrate();

	~MassSpring();

private:
	std::shared_ptr<TetMesh> tetMesh; 

	// Physical parameters
	std::vector<std::pair<uint32_t, uint32_t>> springs;
	std::vector<float> restLengths;

	Eigen::Vector3f gravity = Eigen::Vector3f(0.0f, -9.8f, 0.0f);
	float dt = 1.0f / 60.0f;
	float springStiffness = 5000;
	float damping = 0.0f;
	float perVertMass = 1.0f;
	float collisionPenalty = 30000000.0f;
	bool isPinFirstVertex = false;
	bool cgSolver = true;

	Eigen::VectorXf positions,force,velocity;
	
	Eigen::SparseMatrix<float> massMatrix,jacobian;

	std::unordered_map<uint32_t, int> matrixToValuesMap;

	Eigen::ConjugateGradient<Eigen::SparseMatrix<double>> solver;
	Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> ldlt_solver;
};