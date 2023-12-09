#pragma once

#include "includes.h"
#include "sceneObject.h"

class Camera : public SceneObject
{
public:
	Camera(Eigen::Vector3f lookAtPos = Eigen::Vector3f::Zero(), float fov = 45, float near = 0.01, float far = 1000);
	Eigen::Matrix4f viewMatrix() const;
	Eigen::Matrix4f projectionMatrix(int WindowWidth, int WindowHeight) const;
	void rotateCamera(const Eigen::Vector2d& delta);
	void zoom(float dist);
	void panCamera(const Eigen::Vector2d& delta);
	void switchProjectionType(bool isPerspective = true);

public:
	bool isPerspective = true;
private:
	Eigen::Vector3f lookAtPosition,rotation,dragStart;
	float theta // angle around y axis
		, phi; // angle around x axis
	float FOV,nearPlane,farPlane,distance;
};