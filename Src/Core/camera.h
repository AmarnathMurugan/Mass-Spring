#pragma once

#include "includes.h"
#include "sceneObject.h"

class Camera : public SceneObject
{
public:
	Camera(float fov = 45, float near = 0.01, float far = 1000);
	Eigen::Matrix4f viewMatrix() const;
	Eigen::Matrix4f projectionMatrix(int WindowWidth, int WindowHeight) const;
	void switchProjectionType(bool isPerspective = true);

public:
	bool isPerspective = true;
private:
	float FOV,nearPlane,farPlane;
};