#pragma once

#include "includes.h"
#include "component.h"

class SceneObject : public std::enable_shared_from_this<SceneObject>
{
public:	
	SceneObject() : name("sceneObject") {}
    virtual void render() {}
    virtual void addComponent(std::shared_ptr<Component> component);
    virtual void start(const EngineState& engineState);
    virtual void update(const EngineState& engineState);
    void fixedUpdate(const EngineState& engineState);

   public:
    Transform transform;
    std::string name;
    std::vector<std::shared_ptr<Component>> components;
    bool isActive=true, isRenderable=true;    
};

