#pragma once
#include "sceneObject.h"
#include "component.h"

class ArcBall : public Component
{
	public:
		Eigen::Vector3f lookAtPosition = Eigen::Vector3f::Zero();
		Eigen::Vector3f *lookAtPositionPtr = nullptr;
		bool isPan = true;
		bool isZoom = true;
		bool isLookAt = true;
		float rotationSpeed = 0.005f;
		float zoomSpeed = 0.02f;
		float panSpeed = 0.04f;

	private:
		Eigen::Vector3f	dragStart;
		float theta = 0; // angle with xy plane
		float phi = 0; // angle around y axis
		float distance = 5;
	public:
		ArcBall(Eigen::Vector3f *_lookAtPositionPtr = nullptr);
		void start(const EngineState& engineState) override;
		void update(const EngineState& engineState) override;
		void rotate(const Eigen::Vector2d& delta);
		void zoom(float dist);
		void pan(const Eigen::Vector2d& delta);
};