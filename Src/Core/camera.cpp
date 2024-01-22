#include "camera.h"

Camera::Camera(Eigen::Vector3f _target,float fov, float near, float far):lookAtPosition(_target), FOV(fov), nearPlane(near), farPlane(far)
{
}

Eigen::Matrix4f Camera::viewMatrix() const
{
	return this->transform.matrix().inverse();
}

Eigen::Matrix4f Camera::projectionMatrix(int WindowWidth,int WindowHeight) const
{
	Eigen::Matrix4f projection = Eigen::Matrix4f::Zero();
	float aspect = WindowWidth / (float)WindowHeight;
	if (this->isPerspective)
	  return CustomUtils::perspectiveProjection(this->FOV, aspect, this->nearPlane, this->farPlane);
	else
	{
		float distance = (this->transform.position - this->lookAtPosition).norm();
		return CustomUtils::orthographicProjection(distance, this->FOV, aspect, this->nearPlane, this->farPlane);
	}
}

void Camera::switchProjectionType(bool isPerspective)
{ 
	this->isPerspective = isPerspective;
}