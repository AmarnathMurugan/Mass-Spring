#pragma once

#include "includes.h"
#include "Component.h"

class SceneObject
{
public:	
	SceneObject() : transform(Transform::Identity()), name("sceneObject") {}
    virtual void render() {}
    virtual void AddComponent(std::shared_ptr<Component> component);
    void Start();
    virtual void update(const EngineState& engineState);
    void fixedUpdate(const EngineState& engineState);
    Eigen::Matrix4f getModelMatrix();

   public:
    Transform transform;
    std::string name;
    std::vector<std::shared_ptr<Component>> components;
    bool isActive=true, isRenderable=true;    
};

