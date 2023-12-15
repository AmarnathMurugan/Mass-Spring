#include "engine.h"
#include "tet_mesh_boundary.h"


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
	this->scene.cam = std::make_shared<Camera>(Eigen::Vector3f(0,.2,0), 5 ,45);

	std::shared_ptr<Shader> unlitShader = std::make_shared<Shader>(
		std::unordered_map<ShaderType, std::string>
	{
		{ShaderType::VertexShader, "resources/Shaders/Unlit/unlit.vert"},
		{ ShaderType::FragmentShader,"resources/Shaders/Unlit/unlit.frag" }
	});

	std::shared_ptr<Shader> blinnPhongShader = std::make_shared<Shader>(
		std::unordered_map<ShaderType, std::string>
	{
		{ShaderType::VertexShader, "resources/Shaders/BlinnPhong/blinnPhong.vert"},
		{ ShaderType::FragmentShader,"resources/Shaders/BlinnPhong/blinnPhong.frag" }
	});

	std::shared_ptr<Shader> blinnPhongShaderDobulePrecision = std::make_shared<Shader>(
		std::unordered_map<ShaderType, std::string>
	{
		{ShaderType::VertexShader, "resources/Shaders/BlinnPhong/blinnPhongDoublePrecision.vert"},
		{ ShaderType::FragmentShader,"resources/Shaders/BlinnPhong/blinnPhong.frag" }
	});
	

	SurfaceProperties blinnPhongProperties = {
		.diffuseColor = Eigen::Vector3f(1.0f, 0.5f, 0.2f),
		.specularColor = Eigen::Vector3f(1.0f, 1.0f, 1.0f),
		.shininess = 50.0f,
	};
	std::shared_ptr<BlinnPhongMaterial> tetBlinnMat = std::make_shared<BlinnPhongMaterial>(blinnPhongShaderDobulePrecision, blinnPhongProperties);

	std::shared_ptr<TetMesh> tetMesh = std::make_shared<TetMesh>();
	loadTetMesh("resources/Models/TetMesh/Bunny/bunny", tetMesh->tetData.vertices, tetMesh->tetData.tetrahedra, tetMesh->tetData.faces, tetMesh->tetData.faceInteriorVertexIndices, tetMesh->tetData.numBdryVertices);
	tetMesh->initTetMesh(Eigen::Vector3d(0.0,1.,0.0));
	this->scene.addSceneObject(tetMesh, tetBlinnMat);

	std::shared_ptr<MassSpring> massSpring = std::make_shared<MassSpring>(tetMesh);
	tetMesh->AddComponent(massSpring);

	
	blinnPhongProperties.diffuseColor = Eigen::Vector3f(0.5f, 1.0f, 0.2f);
	blinnPhongProperties.shininess = 500.0f;
	std::shared_ptr<BlinnPhongMaterial> planeMat = std::make_shared<BlinnPhongMaterial>(blinnPhongShader, blinnPhongProperties);
	std::shared_ptr<TriangularMesh> plane = std::make_shared<TriangularMesh>();
	CustomUtils::importObjModel("resources/Models/plane.obj", false, plane->vertexData.position, plane->vertexData.normal, plane->faceIndices, plane->vertAdjacency);
	plane->generateBuffers();
	plane->transform.scale(Eigen::Vector3f(3, 3, 3));
	this->scene.addSceneObject(plane, planeMat);

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
	this->physicsSettings.start = std::chrono::high_resolution_clock::now();
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
		case GLEQ_SCROLLED:
			this->scene.cam->zoom(-event.scroll.y * 1.5f);
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
	
	bool isAltPressed = (this->keyboardState.held.find(GLFW_KEY_LEFT_ALT) != this->keyboardState.held.end()) ||
		(this->keyboardState.held.find(GLFW_KEY_RIGHT_ALT) != this->keyboardState.held.end());
	
	if (isAltPressed && this->mouseState.isLeftDown)
		this->scene.cam->rotateCamera(this->mouseState.deltaPos);
	
	if(isAltPressed && this->mouseState.isRightDown)
		this->scene.cam->zoom(this->mouseState.deltaPos.y() * 0.2);

	if(isAltPressed && this->mouseState.isMiddleDown)
		this->scene.cam->panCamera(this->mouseState.deltaPos);
}

void Engine::update()
{
	this->mouseState.prevPos = this->mouseState.curPos;
	glfwGetCursorPos(this->window, &this->mouseState.curPos.x(), &this->mouseState.curPos.y());
	this->mouseState.deltaPos = this->mouseState.curPos - this->mouseState.prevPos;

	this->handleInteractions();

	// Set global render state parameters
	this->scene.renderState.viewMatrix = this->scene.cam->viewMatrix();
	this->scene.renderState.projectionMatrix = this->scene.cam->projectionMatrix(this->scene.renderState.windowWidth, this->scene.renderState.windowHeight);
	Eigen::Matrix4f VP = this->scene.renderState.projectionMatrix * this->scene.renderState.viewMatrix;

	// iterate through all shaders and render the scene objects
	for (auto& [shader, sceneObjs] : this->scene.shaderSceneObjectMapping)
	{
		shader->bind();

		// Set matrix uniforms
		shader->setUniform("uView", this->scene.renderState.viewMatrix);
		shader->setUniform("uProjection", this->scene.renderState.projectionMatrix);
		shader->setUniform("uVP", VP);

		// Set lighting uniforms
		shader->setUniform("uLightDir", this->scene.renderState.lightDir);
		Eigen::Vector4f lightDir4f;
		lightDir4f.head<3>() = this->scene.renderState.lightDir;
		lightDir4f.w() = 0.0f;
		Eigen::Vector3f viewSpaceLightDir = (this->scene.renderState.viewMatrix * lightDir4f).head<3>();
		shader->setUniform("uViewLightDir", viewSpaceLightDir.normalized());
		shader->setUniform("uLightColor", this->scene.renderState.lightColor);
		shader->setUniform("uLightIntensity", this->scene.renderState.lightIntensity);
		shader->setUniform("uAmbientColor", this->scene.renderState.ambientColor);
		shader->setUniform("uAmbientIntensity", this->scene.renderState.ambientIntensity);

		for (auto& sceneObj : sceneObjs)
		{
			if (!sceneObj->isActive)
				continue;
			sceneObj->update();
			if (!sceneObj->isRenderable)
				continue;
			this->scene.sceneObjectMaterialMapping[sceneObj]->use();
			Eigen::Matrix4f modelMatrix = sceneObj->getModelMatrix();
			Eigen::Matrix4f MV = this->scene.renderState.viewMatrix * modelMatrix;
			Eigen::Matrix4f MVP = VP * modelMatrix;
			Eigen::Matrix3f normalMatrix = MV.block<3, 3>(0, 0).inverse().transpose();
			this->scene.sceneObjectMaterialMapping[sceneObj]->shader->setUniform("uModel", modelMatrix);
			this->scene.sceneObjectMaterialMapping[sceneObj]->shader->setUniform("uMV", MV);
			this->scene.sceneObjectMaterialMapping[sceneObj]->shader->setUniform("uMVP", MVP);
			this->scene.sceneObjectMaterialMapping[sceneObj]->shader->setUniform("uNormalMatrix", normalMatrix);
			sceneObj->render();
		}
	}

	// Update physics
	std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - this->physicsSettings.start;
	if (elapsed.count() > this->physicsSettings.fixedDeltaTime)
	{
		for (auto& sceneObj : this->scene.sceneObjects)
		{
			if (!sceneObj->isActive)
				continue;
			sceneObj->fixedUpdate(this->physicsSettings.fixedDeltaTime);
		}
		this->physicsSettings.start = std::chrono::high_resolution_clock::now();
	}
}

void Engine::stop()
{
	this->scene.clear();
}


