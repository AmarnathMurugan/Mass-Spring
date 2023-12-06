#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H
#include "Components/Component.h"

extern std::vector<std::shared_ptr<material>> materials;

class DirectionalLight : public Component
{
public:
	DirectionalLight() :intensity(1) {}
	void Start() {}
	void Update() {}
	void RotateLight(float x, float y);
public:
	float intensity;
};

void DirectionalLight::RotateLight(float x, float y)
{
	sceneObject->position = glm::rotate(x, Eigen::Vector3f(1.0, 0, 1.0)) * glm::vec4(sceneObject->position, 1.0);
	sceneObject->position = glm::rotate(y, Eigen::Vector3f(0.0, 1.0, 0)) * glm::vec4(sceneObject->position, 1.0);
	sceneObject->TriggerMvpEvent();
	for (auto mat : materials)
		mat->OnLightChange();
}


#endif // !DIRECTIONALLIGHT_H

