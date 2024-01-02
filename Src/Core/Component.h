#pragma once

class SceneObject;

class component {
 public:
  virtual void Start() = 0;
  virtual void update(const EngineState& engineState) = 0;
  virtual void fixedUpdate(const EngineState& engineState) = 0;
 
 public:
	bool isEnabled = true;
};