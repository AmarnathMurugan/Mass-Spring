#pragma once

#include "sceneObject.h"
#include "gfx.h"

class Camera : public SceneObject
{

public:
	CameraState* cameraState = nullptr;


public:
	Camera(CameraState* _cameraState);
	Eigen::Matrix4f viewMatrix() const;
	Eigen::Matrix4f projectionMatrix(int WindowWidth, int WindowHeight) const;
	void switchProjectionType(bool isPerspective = true);
};