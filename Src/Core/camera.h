#pragma once

#include "includes.h"
#include "sceneObject.h"

class Camera : public SceneObject
{
public:
	Camera(Eigen::Vector3f lookAtPos = Eigen::Vector3f::Zero(), float fov = 45, float near = 0.01, float far = 1000);
	Eigen::Matrix4f viewMatrix() const;
	Eigen::Matrix4f projectionMatrix(int WindowWidth, int WindowHeight) const;
	void RotateCamera(float x, float y);
	void MoveAlongRay(float dist);
	//void PanCamera();
	void SwitchProjectionType(bool isPerspective = true);
	void RegistersceneObject(SceneObject* o);

public:
	std::vector<SceneObject*> sceneObjects;
private:
	Eigen::Vector3f lookAtPosition;
	float FOV,nearPlane,farPlane;
	bool isPerspective = true;
};