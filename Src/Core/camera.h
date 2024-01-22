#pragma once

#include "includes.h"
#include "sceneObject.h"

class Camera : public SceneObject
{
public:
	Camera(Eigen::Vector3f _target = Eigen::Vector3f::Zero(), float fov = 45, float near = 0.01, float far = 1000);
	Eigen::Matrix4f viewMatrix() const;
	Eigen::Matrix4f projectionMatrix(int WindowWidth, int WindowHeight) const;
	void switchProjectionType(bool isPerspective = true);

public:
	bool isPerspective = true;
	Eigen::Vector3f lookAtPosition;
private:
	float FOV,nearPlane,farPlane;
};