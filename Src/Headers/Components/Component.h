#ifndef COMPONENT_H
#define COMPONENT_H

class SceneObject;

class Component {
 public:
  virtual void Start() = 0;
  virtual void Update() = 0;
 
 public:
	bool isEnabled = true;
	SceneObject* sceneObject;
};


#endif  // !COMPONENT_H
