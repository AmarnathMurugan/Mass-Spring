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
	void calculateJacobian();
	void integrate(float dt);

	~MassSpring();

private:
	std::shared_ptr<TetMesh> tetMesh; 

	// Physical parameters
	std::vector<std::pair<uint32_t, uint32_t>> springs;
	std::vector<float> restLengths;

	Eigen::Vector3f gravity = Eigen::Vector3f(0.0f, -9.8f, 0.0f);
	float springStiffness = 1000.0f;
	float damping = 10.0f;
	float totalMass = 16.0f;

	Eigen::VectorXf positions,force,velocity;
	
	Eigen::SparseMatrix<float> massMatrix,jacobian;

	Eigen::ConjugateGradient<Eigen::SparseMatrix<float>> solver;
};