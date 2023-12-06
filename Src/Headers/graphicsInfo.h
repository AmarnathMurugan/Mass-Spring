#pragma once
#include <Eigen/Dense>
#include <Eigen/Core>

#define GL_INVALID_INDEX -1

struct RenderState
{
	Eigen::Matrix4f viewMatrix,projectionMatrix;
	Eigen::Vector3f cameraPosition;
	Eigen::Vector3f ambientColor = Eigen::Vector3f(1,1,1);
	Eigen::Vector3f clearColor = Eigen::Vector3f(0,0,0);
	int windowWidth = 960,windowHeight = 590;
	float ambientIntensity = 0.2f;
};