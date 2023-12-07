#pragma once
#include "includes.h"
#include "camera.h"
#include "triangularMesh.h"
#include "unlitMaterial.h"
#include "gleq.h"


class Engine
{
public:
	Engine(GLFWwindow* _window);
	void initScene();
	void start();
	void handleEvent(const GLEQevent& event);
	void handleInteractions();
	void update();
	void stop();
private:

private:
	GLFWwindow* window;
	struct Scene
	{
		std::vector<std::shared_ptr<SceneObject>> sceneObjects;
		std::vector<std::shared_ptr<Shader>> shaders;
		std::unordered_map<std::shared_ptr<SceneObject>, std::shared_ptr<Material>> sceneObjectMaterialMapping;
		std::shared_ptr<Camera> cam;
		RenderState renderState;

		void clear()
		{
			sceneObjects.clear();
			shaders.clear();
			sceneObjectMaterialMapping.clear();
			cam.reset();
		}
	}scene;

	struct KeyboardState
	{
		std::unordered_set<int> down,held,released;
	}keyboardState;

};
