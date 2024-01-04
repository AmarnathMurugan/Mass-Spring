#include "arcBallComp.h"

void ArcBall::start(const EngineState& engineState)
{
	this->sceneObject->transform.position = this->lookAtPosition + CustomUtils::spherePoint(this->theta, this->phi) * this->distance;
	Eigen::Vector3f pos = this->sceneObject->transform.position;
}

void ArcBall::update(const EngineState& engineState)
{
	if(this->isZoom)
		this->zoom(engineState.mouse.scroll * -5);

	bool isAltPressed = (engineState.keyboard.held.find(GLFW_KEY_LEFT_ALT) != engineState.keyboard.held.end()) ||
		(engineState.keyboard.held.find(GLFW_KEY_RIGHT_ALT) != engineState.keyboard.held.end());
	
	if(!isAltPressed)
		return;

	if (engineState.mouse.isLeftDown)
		this->rotate(engineState.mouse.deltaPos);

	if (engineState.mouse.isRightDown && this->isZoom)
		this->zoom(engineState.mouse.deltaPos.y());

	if (engineState.mouse.isMiddleDown && this->isPan)
		this->pan(engineState.mouse.deltaPos);
}

void ArcBall::rotate(const Eigen::Vector2d& delta)
{	
	// theta is clamped to +/- 89.99 degs avoid singularity at poles
	this->theta = CustomUtils::clamp(this->theta + (float)delta.y() * this->rotationSpeed, -PI_F * 0.499f, PI_F * 0.499f);
	this->phi = this->phi - delta.x() * this->rotationSpeed;
	this->sceneObject->transform.position = this->lookAtPosition + CustomUtils::spherePoint(this->theta, this->phi) * this->distance;
	if (!this->isLookAt)
		return;
	Eigen::Vector3f dir = (this->lookAtPosition - this->sceneObject->transform.position).normalized();
	Eigen::Vector3f up = Eigen::Vector3f(0, 1, 0);
	Eigen::Vector3f right = dir.cross(up).normalized();
	Eigen::Vector3f correctedUp = right.cross(dir).normalized();
	Eigen::Matrix3f lookAtRotation;
	lookAtRotation << right, correctedUp, -dir;
	this->sceneObject->transform.rotation = Eigen::Quaternionf(lookAtRotation);
}

void ArcBall::zoom(float delta)
{
	Eigen::Vector3f dir = this->sceneObject->transform.position - lookAtPosition;
	distance += delta * this->zoomSpeed;
	distance = std::max(distance, 0.1f);
	this->sceneObject->transform.position = lookAtPosition + dir.normalized() * distance;
}

void ArcBall::pan(const Eigen::Vector2d& delta)
{	
	const Eigen::Vector3f dir = Eigen::Vector3f(-delta.x(), delta.y(), 0.0) * this->panSpeed / this->distance;
	const  Eigen::Matrix4f view = this->sceneObject->transform.matrix().inverse();
	this->lookAtPosition += view.block<3, 3>(0, 0).transpose() * dir;
	rotate(Eigen::Vector2d(0, 0));
}
