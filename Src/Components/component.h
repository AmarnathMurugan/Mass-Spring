#pragma once
#include "gfx.h"

class SceneObject;

class Component {
 public:
  virtual void start(const EngineState& engineState) = 0;
  virtual void update(const EngineState& engineState) = 0;
  virtual void fixedUpdate(const EngineState& engineState) {}
 
 public:
	bool isEnabled = true;
	std::shared_ptr<SceneObject> sceneObject;
};