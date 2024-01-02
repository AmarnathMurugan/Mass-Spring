#pragma once

class SceneObject;

class Component {
 public:
  virtual void start(const EngineState& engineState) = 0;
  virtual void update(const EngineState& engineState) = 0;
  virtual void fixedUpdate(const EngineState& engineState) = 0;
 
 public:
	bool isEnabled = true;
};