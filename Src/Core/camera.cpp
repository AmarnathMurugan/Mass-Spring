#include "camera.h"

Camera::Camera(float fov, float near, float far):FOV(fov),nearPlane(near),farPlane(far)
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
		float zoom = CustomUtils::radians(this->FOV) * 5.0f;
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

void Camera::switchProjectionType(bool isPerspective)
{ 
	this->isPerspective = isPerspective;
}