#include "sceneObject.h"


void SceneObject::AddComponent(std::shared_ptr<Component> comp)
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

void SceneObject::update()
{
    for (auto component : components)
    {
        if (component->isEnabled)
            component->update();
    }
}

void SceneObject::fixedUpdate(float dt)
{
    for (auto component : components)
    {
		if (component->isEnabled)
			component->fixedUpdate(dt);
	}
}

Eigen::Matrix4f SceneObject::getModelMatrix()
{
    return transform.matrix();
}
