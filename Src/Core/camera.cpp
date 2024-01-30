#include "camera.h"

Camera::Camera(CameraState* _cameraState):cameraState(_cameraState)
{
	this->cameraState->transform = &transform;
}

Eigen::Matrix4f Camera::viewMatrix() const
{
	return this->transform.matrix().inverse();
}

Eigen::Matrix4f Camera::projectionMatrix(int WindowWidth,int WindowHeight) const
{
	Eigen::Matrix4f projection = Eigen::Matrix4f::Zero();
	float aspect = WindowWidth / (float)WindowHeight;
	if (this->cameraState->isPerspective)
	  return CustomUtils::perspectiveProjection(this->cameraState->FOV, aspect, this->cameraState->nearPlane, this->cameraState->farPlane);
	else
	{
		float distance = (this->transform.position - this->cameraState->lookAtPosition).norm();
		return CustomUtils::orthographicProjection(distance, this->cameraState->FOV, aspect, this->cameraState->nearPlane, this->cameraState->farPlane);
	}
}

void Camera::switchProjectionType(bool isPerspective)
{ 
	this->cameraState->isPerspective = isPerspective;
}