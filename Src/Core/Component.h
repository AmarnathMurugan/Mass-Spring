#pragma once

class SceneObject;

class Component {
 public:
  virtual void Start() = 0;
  virtual void update() = 0;
  virtual void fixedUpdate(float dt) {}
 
 public:
	bool isEnabled = true;
};