#include "camera.h"

Camera::Camera(Eigen::Vector3f lookAtPos, float fov, float near, float far):lookAtPosition(lookAtPos),FOV(fov),nearPlane(near),farPlane(far)
{
	this->transform.matrix().setIdentity();
	this->transform.translate(lookAtPos + Eigen::Vector3f(0,0,5));
	this->isPerspective = true;
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
	{
		float tanhalf = tanf(CustomUtils::radians(FOV) / 2.0f);
		projection(0, 0) = 1.0f / (aspect * tanhalf);
		projection(1, 1) = 1.0f / tanhalf;
		projection(2, 2) = -(this->farPlane + this->nearPlane) / (this->farPlane - this->nearPlane);
		projection(2, 3) = -2.0f * this->farPlane * this->nearPlane / (this->farPlane - this->nearPlane);
		projection(3, 2) = -1.0f;
	}
	else
	{
		float zoom = 1.0f;
		float left = -aspect * zoom * 0.5f;
		float right = aspect * zoom * 0.5f;
		float bottom = -zoom * 0.5f;
		float top = zoom * 0.5f;
		projection(0, 0) = 2 / (right - left);
		projection(1, 1) = 2 / (top - bottom);
		projection(2, 2) = -2 / (farPlane - nearPlane);
		projection(0, 3) = -(right + left) / (right - left);
		projection(1, 3) = -(top + bottom) / (top - bottom);
		projection(2, 3) = -(farPlane + nearPlane) / (farPlane - nearPlane);
		projection(3, 3) = 1;
	}
	return projection;
}


void Camera::rotateCamera(const Eigen::Vector2d& delta)
{
	// Rotate around lookAtPosition using spherical coordinates
	Eigen::Vector3f dir = this->transform.translation() - lookAtPosition;
	float r = dir.norm();
	float theta = acos(dir.y() / r);
	float phi = atan2(dir.z(), dir.x());
	theta += delta.y() * 0.01;
	phi += delta.x() * 0.01;
	if (theta < 0.01)
		theta = 0.01;
	if (theta > PI_F - 0.01)
		theta = PI_F - 0.01;
	dir.x() = r * sin(theta) * cos(phi);
	dir.y() = r * cos(theta);
	dir.z() = r * sin(theta) * sin(phi);
	this->transform.translation() = lookAtPosition + dir;
	// set rotation to theta and phi
	Eigen::Matrix3f rot = Eigen::Matrix3f::Zero();
	rot = Eigen::AngleAxisf(theta, Eigen::Vector3f::UnitY()) * Eigen::AngleAxisf(phi, Eigen::Vector3f::UnitZ());
this->transform.linear() = rot;
}

void Camera::panCamera(const Eigen::Vector2d& delta)
{
	Eigen::Vector3f dir = this->transform.translation() - lookAtPosition;
	dir.normalize();
	Eigen::Vector3f up = Eigen::Vector3f(0, 1, 0);
	Eigen::Vector3f right = dir.cross(up);
	right.normalize();
	Eigen::Vector3f translation = Eigen::Vector3f::Zero();
	translation += right * delta.x();
	translation -= up * delta.y();
	translation *= 0.01;
	this->transform.translate(translation);
	lookAtPosition += translation;
}

void Camera::moveAlongRay(float dist)
{
	Eigen::Vector3f dir = this->transform.translation() - lookAtPosition;
	dir.normalize();
	this->transform.translate(dir * dist * 0.2f);
}

void Camera::switchProjectionType(bool isPerspective)
{ 
	this->isPerspective = isPerspective;
}