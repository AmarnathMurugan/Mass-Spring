#include "sceneObject.h"


void SceneObject::addComponent(std::shared_ptr<Component> comp)
{
    components.emplace_back(comp);
}

void SceneObject::start(const EngineState& engineState)
{
    for (auto component : components)
    {
        component->start(engineState);
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
