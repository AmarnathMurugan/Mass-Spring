#include "camera.h"

Camera::Camera(Eigen::Vector3f lookAtPos,float _distance, float fov, float near, float far):lookAtPosition(lookAtPos),FOV(fov),nearPlane(near),farPlane(far),distance(_distance)
{
	this->transform.matrix().setIdentity();
	this->theta = this->phi = 0;
	this->transform.position = this->lookAtPosition + CustomUtils::spherePoint(this->theta,this->phi) * this->distance;
	Eigen::Vector3f pos = this->transform.position;
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
		float zoom = CustomUtils::radians(this->FOV) * this->distance;
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
	float sensitivity = 0.005f;
	this->theta = CustomUtils::clamp(this->theta + (float)delta.y() * sensitivity, -PI_F * 0.499f, PI_F * 0.499f);
	this->phi = this->phi - delta.x() * sensitivity;
	this->transform.position = this->lookAtPosition + CustomUtils::spherePoint(this->theta, this->phi) * this->distance;	
	Eigen::Vector3f dir = (this->lookAtPosition - this->transform.position).normalized();
	Eigen::Vector3f up = Eigen::Vector3f(0, 1, 0);
	Eigen::Vector3f right = dir.cross(up).normalized();
	Eigen::Vector3f correctedUp = right.cross(dir).normalized();
	Eigen::Matrix3f lookAtRotation;
	lookAtRotation << right, correctedUp, -dir;
	this->transform.rotation = Eigen::Quaternionf(lookAtRotation);
}

void Camera::panCamera(const Eigen::Vector2d& delta)
{
	float sensitivity = 0.04f;
	const Eigen::Vector3f dir = Eigen::Vector3f(-delta.x(), delta.y(),0.0) * sensitivity / this->distance;
	const  Eigen::Matrix4f view = this->viewMatrix();
	this->lookAtPosition += view.block<3, 3>(0, 0).transpose() * dir;
	rotateCamera(Eigen::Vector2d(0,0));
}

void Camera::zoom(float delta)
{
	Eigen::Vector3f dir = this->transform.position - lookAtPosition;
	distance += delta * 0.1;
	distance = std::max(distance, 0.1f);
	this->transform.position = lookAtPosition + dir.normalized() * distance;
}

void Camera::switchProjectionType(bool isPerspective)
{ 
	this->isPerspective = isPerspective;
}