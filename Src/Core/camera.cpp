#include "camera.h"

Camera::Camera(Eigen::Vector3f lookAtPos, float fov, float near, float far):lookAtPosition(lookAtPos),FOV(fov),nearPlane(near),farPlane(far)
{
	this->transform.matrix().setIdentity();
	this->transform.translate(lookAtPos - Eigen::Vector3f(0, 0, 5));
	this->isPerspective = true;
}

Eigen::Matrix4f Camera::viewMatrix() const
{
	return this->transform.matrix();
}

Eigen::Matrix4f Camera::projectionMatrix(int WindowWidth,int WindowHeight) const
{
	Eigen::Matrix4f projection = Eigen::Matrix4f::Zero();
	float aspect = WindowWidth / (float)WindowHeight;
	if (this->isPerspective)
	{
		float tanHalfFOV = tan(FOV / 2);
		projection = Eigen::Matrix4f::Identity();
		projection(0, 0) = 1 / (aspect * tanHalfFOV);
		projection(1, 1) = 1 / tanHalfFOV;
		projection(2, 2) = -(farPlane + nearPlane) / (farPlane - nearPlane);
		projection(2, 3) = -(2 * farPlane * nearPlane) / (farPlane - nearPlane);
		projection(3, 2) = -1;
		projection(3, 3) = 0;
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


void Camera::RotateCamera(float x, float y)
{
	// rotate around lookAtPosition on sphere of radius distance
	Eigen::Vector3f dir = this->transform.translation() - lookAtPosition;
	float dist = dir.norm();
	dir.normalize();
	Eigen::Vector3f right = dir.cross(Eigen::Vector3f(0,1,0));
	right.normalize();
	Eigen::Vector3f up = right.cross(dir);
	up.normalize();
	Eigen::Matrix3f rot;
	rot.col(0) = right;
	rot.col(1) = up;
	rot.col(2) = dir;
	Eigen::Matrix3f rotX = Eigen::AngleAxisf(x, up).toRotationMatrix();
	Eigen::Matrix3f rotY = Eigen::AngleAxisf(y, right).toRotationMatrix();
	rot = rotY * rotX * rot;
	this->transform.matrix().block<3, 3>(0, 0) = rot;
	this->transform.translate(lookAtPosition + dir * dist);
}

void Camera::moveAlongRay(float dist)
{
	Eigen::Vector3f dir = this->transform.translation() - lookAtPosition;
	dir.normalize();
	this->transform.translate(dir * dist);
}

void Camera::switchProjectionType(bool isPerspective)
{ 
	this->isPerspective = isPerspective;
}