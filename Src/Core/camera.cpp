#include "camera.h"

Camera::Camera(Eigen::Vector3f lookAtPos, float fov, float near, float far):lookAtPosition(lookAtPos),FOV(fov),nearPlane(near),farPlane(far)
{
	this->transform.matrix().setIdentity();
	this->distance = 5;
	this->theta = PI_F / 2.0f;
	this->phi = 0;
	Eigen::Vector3f test = CustomUtils::sphericalToCartesian(this->theta, this->phi);
	this->transform.translation() = lookAtPosition + test * distance;
	this->isPerspective = true;
}

Eigen::Matrix4f Camera::viewMatrix() const
{
	Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

	Eigen::Vector3f eye = this->transform.translation();
	Eigen::Vector3f center = this->lookAtPosition;
	Eigen::Vector3f up = Eigen::Vector3f(0, 1, 0);
	Eigen ::Vector3f f = (center - eye).normalized();
	Eigen::Vector3f s = f.cross(up).normalized();
	Eigen::Vector3f u = s.cross(f);
	view(0, 0) = s.x();
	view(1, 0) = s.y();
	view(2, 0) = s.z();
	view(0, 1) = u.x();
	view(1, 1) = u.y();
	view(2, 1) = u.z();
	view(0, 2) = -f.x();
	view(1, 2) = -f.y();
	view(2, 2) = -f.z();
	view(0, 3) = -s.dot(eye);
	view(1, 3) = -u.dot(eye);
	view(2, 3) = f.dot(eye);
	return view;


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
	this->theta += delta.y() * 0.001;
	this->phi -= delta.x() * 0.001;
	if (theta < 0.01)
		theta = 0.01;
	if (theta > PI_F - 0.01)
		theta = PI_F - 0.01;
	Eigen::Vector3f dir = CustomUtils::sphericalToCartesian(this->theta, phi);
	this->transform.translation() = lookAtPosition + dir * distance;
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
	translation += up * delta.y();
	translation *= 0.001 * distance;
	this->transform.translate(translation);
	lookAtPosition += translation;
}

void Camera::moveAlongRay(float delta)
{
	Eigen::Vector3f dir = this->transform.translation() - lookAtPosition;
	distance += delta * 0.1;
	distance = std::max(distance, 0.1f);
	this->transform.translation() = lookAtPosition + dir.normalized() * distance;
}

void Camera::switchProjectionType(bool isPerspective)
{ 
	this->isPerspective = isPerspective;
}