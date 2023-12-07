#include "engine.h"


Engine::Engine(GLFWwindow* _window): window(_window)
{
	// Init OpenGL Parameters
	glClearColor(this->scene.renderState.clearColor.x(), this->scene.renderState.clearColor.y(), this->scene.renderState.clearColor.z(), 1.0f);
	glViewport(0, 0, this->scene.renderState.windowWidth, this->scene.renderState.windowHeight);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

	this->initScene();
}

void Engine::initScene()
{
	this->scene.cam = std::make_shared<Camera>(Eigen::Vector3f::Zero(), 50);
	std::shared_ptr<Shader> unlitShader = std::make_shared<Shader>(
		std::unordered_map<ShaderType, std::string>
	{
		{ShaderType::VertexShader, "resources/Shaders/Unlit/unlit.vert"},
		{ ShaderType::FragmentShader,"resources/Shaders/Unlit/unlit.frag" }
	});

	this->scene.shaders.emplace_back(unlitShader);
	std::shared_ptr<UnlitMaterial> unlitMaterial = std::make_shared<UnlitMaterial>(unlitShader, Eigen::Vector3f(1.0f, 0.5f, 0.2f));

	std::shared_ptr<TriangularMesh> mesh = std::make_shared<TriangularMesh>();
	this->scene.sceneObjects.emplace_back(mesh);
	this->scene.sceneObjectMaterialMapping[mesh] = unlitMaterial;

	// Create quad with screen-space coordinates center of screen
	mesh->vertexData.position.resize(4, 3);
	mesh->vertexData.position << -0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f;
	mesh->vertexData.normal.resize(4, 3);
	mesh->vertexData.normal << 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.f,
		0.0f, 0.0f, 1.f,
		0.0f, 0.0f, 1.f;
	mesh->faceIndices.resize(2, 3);
	mesh->faceIndices << 0, 1, 2,
		0, 2, 3;
	mesh->generateBuffers();
}


void Engine::start()
{
	for (int i = 0; i < this->scene.sceneObjects.size(); i++)
	{
		if (!this->scene.sceneObjects[i]->isActive)
			continue;
		this->scene.sceneObjects[i]->Start();
	}
	glfwGetCursorPos(this->window, &this->mouseState.prevPos.x(), &this->mouseState.prevPos.y());
	this->mouseState.curPos = this->mouseState.prevPos;
}

void Engine::handleEvent(const GLEQevent& event)
{
	this->keyboardState.down.clear();
	this->keyboardState.released.clear();

	switch (event.type)
	{
		case GLEQ_WINDOW_RESIZED:
			this->scene.renderState.windowWidth = event.size.width;
			this->scene.renderState.windowHeight = event.size.height;
			glViewport(0, 0, this->scene.renderState.windowWidth, this->scene.renderState.windowHeight);
			break;
		case GLEQ_KEY_PRESSED:
			this->keyboardState.down.insert(event.keyboard.key);
			this->keyboardState.held.insert(event.keyboard.key);
			this->keyboardState.down.insert(event.keyboard.mods);
			this->keyboardState.held.insert(event.keyboard.mods);
			this ->handleInteractions(event.keyboard.key, true);
			break;
		case GLEQ_KEY_RELEASED:
			this->keyboardState.held.erase(event.keyboard.key);
			this->keyboardState.released.insert(event.keyboard.key);
			this->keyboardState.held.erase(event.keyboard.mods);
			this->keyboardState.released.insert(event.keyboard.mods);
			this->handleInteractions(event.keyboard.key, false);
			break;
		case GLEQ_BUTTON_PRESSED:
			this->handleInteractions(event.mouse.button, true);
			switch (event.mouse.button)
			{
			case GLFW_MOUSE_BUTTON_LEFT:
				this->mouseState.isLeftDown = true;
				break;
			case GLFW_MOUSE_BUTTON_RIGHT:
				this->mouseState.isRightDown = true;
				break;
			case GLFW_MOUSE_BUTTON_MIDDLE:
				this->mouseState.isMiddleDown = true;
				break;
			}
			break;
		case GLEQ_BUTTON_RELEASED:
			this->handleInteractions(event.mouse.button, false);
			switch (event.mouse.button)
			{
			case GLFW_MOUSE_BUTTON_LEFT:
				this->mouseState.isLeftDown = false;
				break;
			case GLFW_MOUSE_BUTTON_RIGHT:
				this->mouseState.isRightDown = false;
				break;
			case GLFW_MOUSE_BUTTON_MIDDLE:
				this->mouseState.isMiddleDown = false;
				break;
			}
			break;
	}
}

void Engine::handleInteractions(int key, bool isDown)
{
	switch (key)
	{
		case GLFW_KEY_ESCAPE:
			if (!isDown)			
				glfwSetWindowShouldClose(this->window, GLFW_TRUE);			
			break;
		case GLFW_KEY_P:
			if (!isDown) break;
			this->scene.cam->switchProjectionType(!this->scene.cam->isPerspective);
			break;
		case GLFW_KEY_LEFT_CONTROL:
			break;
	}

	switch (key)
	{
		case GLFW_MOUSE_BUTTON_LEFT:
			break;
	}

}

void Engine::handleInteractions()
{
	bool isCtrlPressed = (this->keyboardState.held.find(GLFW_KEY_LEFT_CONTROL) != this->keyboardState.held.end()) || 
						 (this->keyboardState.held.find(GLFW_KEY_RIGHT_CONTROL) != this->keyboardState.held.end());
	if(isCtrlPressed && this->mouseState.isRightDown)
		this->scene.cam->moveAlongRay(this->mouseState.deltaPos.y() * 0.2);
}

void Engine::update()
{
	this->mouseState.prevPos = this->mouseState.curPos;
	glfwGetCursorPos(this->window, &this->mouseState.curPos.x(), &this->mouseState.curPos.y());
	this->mouseState.deltaPos = this->mouseState.curPos - this->mouseState.prevPos;

	this->handleInteractions();

	// iterate through all shaders and set transformation matrices
	for (int j = 0; j < this->scene.shaders.size(); j++)
	{
		this->scene.shaders[j]->bind();
		Eigen::Matrix4f viewMatrix = this->scene.cam->viewMatrix();
		Eigen::Matrix4f projectionMatrix = this->scene.cam->projectionMatrix(this->scene.renderState.windowWidth, this->scene.renderState.windowHeight);
		Eigen::Matrix4f VP = projectionMatrix * viewMatrix;
		this->scene.shaders[j]->setUniform("uView", viewMatrix);
		this->scene.shaders[j]->setUniform("uProjection", projectionMatrix);
		this->scene.shaders[j]->setUniform("uVP", VP);
	}

	for (int i = 0; i < this->scene.sceneObjects.size(); i++)
	{
		if (!this->scene.sceneObjects[i]->isActive)
			continue;
		this->scene.sceneObjects[i]->Update();
		if (!this->scene.sceneObjects[i]->isRenderable)
			continue;
		this->scene.sceneObjectMaterialMapping[this->scene.sceneObjects[i]]->use();
		this->scene.sceneObjectMaterialMapping[this->scene.sceneObjects[i]]->shader->setUniform("uModel", this->scene.sceneObjects[i]->getModelMatrix());
		this->scene.sceneObjects[i]->render();
	}
}

void Engine::stop()
{
	this->scene.clear();
}


