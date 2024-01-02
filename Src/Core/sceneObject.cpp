#include "sceneObject.h"


void SceneObject::AddComponent(std::shared_ptr<component> comp)
{
    components.emplace_back(comp);
}

void SceneObject::Start()
{
    for (auto component : components)
    {
        component->Start();
    }
}

void SceneObject::update(const EngineState& engineState)
{
    for (auto component : components)
    {
        if (component->isEnabled)
            component->update(engineState);
    }
}

void SceneObject::fixedUpdate(const EngineState& engineState)
{
    for (auto component : components)
    {
		if (component->isEnabled)
			component->fixedUpdate(engineState);
	}
}

Eigen::Matrix4f SceneObject::getModelMatrix()
{
    return transform.matrix();
}
