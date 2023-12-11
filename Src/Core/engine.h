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
	void handleInteractions(int key, bool isDown);
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
		std::unordered_map<std::shared_ptr<Shader>,std::unordered_set<std::shared_ptr<SceneObject>>> shaderSceneObjectMapping;
		std::shared_ptr<Camera> cam;
		RenderState renderState;

		void addSceneObject(std::shared_ptr<SceneObject> sceneObj, std::shared_ptr<Material> material = nullptr)
		{
			sceneObjects.push_back(sceneObj);
			if (material)
			{
				sceneObjectMaterialMapping[sceneObj] = material;
				shaderSceneObjectMapping[material->shader].insert(sceneObj);
			}
		}

		void clear()
		{
			for (auto& [shader, sceneObjs] : shaderSceneObjectMapping)
				sceneObjs.clear();
			shaderSceneObjectMapping.clear();
			sceneObjectMaterialMapping.clear();
			sceneObjects.clear();
			shaders.clear();
			cam.reset();
		}
	}scene;

	struct KeyboardState
	{
		std::unordered_set<int> down,held,released;
	}keyboardState;

	struct MouseState
	{
		bool isLeftDown = false;
		bool isRightDown = false;
		bool isMiddleDown = false;
		Eigen::Vector2d prevPos,curPos,deltaPos;
	}mouseState;

};
