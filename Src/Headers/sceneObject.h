#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include "includes.h"
#include "Components/Component.h"

class SceneObject
{
public:	
	SceneObject() : transform(Transform::Identity()), name("sceneObject") {}
    virtual void render() = 0;
    virtual void AddComponent(std::shared_ptr<Component> component);
    void Start();
    void Update();

   public:
    Transform transform;
    std::string name;
    std::vector<std::shared_ptr<Component>> components;
    bool isActive=true, isRenderable=true;

   protected:
    GLuint VBO = GL_INVALID_INDEX, VAO = GL_INVALID_INDEX,EBO = GL_INVALID_INDEX;
    
};

#endif // !SCENEOBJECT_H

