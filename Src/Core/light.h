#pragma once

#include "component.h"

class Light : public Component
{
public:
	enum class LightType
	{
		Directional,
		Point,
		Spot
	};

	struct LightParameters
	{
		LightType lightType = LightType::Directional;
		// ========= Common Params ==========
		float intensity = 1.0f;
		Eigen::Vector3f color = Eigen::Vector3f(1.0f, 1.0f, 1.0f);

		// ========= Spotlight and point light Params ==========
		float innerRadius = 1.0f;
		float outerRadius = 2.0f;

		// ========= Spotlight & Directional light Params ==========
		Eigen::Vector3f target;
	}lightParameters;


public:
	Light(LightParameters _lightParameters) : Component(), lightParameters(_lightParameters)
	{
	}
	Eigen::Matrix4f getLightViewMatrix();
	Eigen::Matrix4f getLightProjectionMatrix();
	void start(const EngineState& engineState) override;
	void update(const EngineState& engineState) override;
private:

};

