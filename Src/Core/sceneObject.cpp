#include "sceneObject.h"


void SceneObject::AddComponent(std::shared_ptr<Component> comp)
{
    comp->sceneObject = this;
    components.emplace_back(comp);
}

void SceneObject::Start()
{
    for (auto component : components)
    {
        component->Start();
    }
}

void SceneObject::Update()
{
    for (auto component : components)
    {
        if (component->isEnabled)
            component->Update();
    }
}

Eigen::Matrix4f SceneObject::getModelMatrix()
{
    return transform.matrix();
}
