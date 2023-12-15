#pragma once

class SceneObject;

class Component {
 public:
  virtual void Start() = 0;
  virtual void update(const EngineState& engineState) = 0;
  virtual void fixedUpdate(const EngineState& engineState) {}
 
 public:
	bool isEnabled = true;
};