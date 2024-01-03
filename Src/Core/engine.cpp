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

	this->engineState.keyboard = std::make_shared<KeyboardState>();
	this->engineState.mouse = std::make_shared<MouseState>();
	this->engineState.physics = std::make_shared<PhysicsSettings>();


	this->initScene();
}

void Engine::initScene()
{
	this->scene.cam = std::make_shared<Camera>();
	this->scene.cam->addComponent(std::make_shared<ArcBall>());

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

	std::shared_ptr<TetMesh> tetMeshADMM = std::make_shared<TetMesh>();
	loadTetMesh("resources/Models/TetMesh/Bunny/bunny", tetMeshADMM->tetData.vertices, tetMeshADMM->tetData.tetrahedra, tetMeshADMM->tetData.faces, tetMeshADMM->tetData.faceInteriorVertexIndices, tetMeshADMM->tetData.numBdryVertices);
	tetMeshADMM->initTetMesh(Eigen::Vector3d(1.0,3.,0.0));
	this->scene.addSceneObject(tetMeshADMM, tetBlinnMat);
	std::shared_ptr<MassSpringADMM> massSpring = std::make_shared<MassSpringADMM>(tetMeshADMM);	
	tetMeshADMM->addComponent(massSpring);

	std::shared_ptr<TetMesh> tetMeshNewton = std::make_shared<TetMesh>(*tetMeshADMM);
	tetMeshNewton->offsetVertices(Eigen::Vector3d(-2.0, 0.0, 0.0));
	blinnPhongProperties.diffuseColor = Eigen::Vector3f(0.5f, 0.2f, 1.0f);
	std::shared_ptr<BlinnPhongMaterial> tetBlinnMatNewton = std::make_shared<BlinnPhongMaterial>(blinnPhongShaderDobulePrecision, blinnPhongProperties);
	this->scene.addSceneObject(tetMeshNewton, tetBlinnMatNewton);
	std::shared_ptr<MassSpring> massSpringNewton = std::make_shared<MassSpring>(tetMeshNewton);
	tetMeshNewton->addComponent(massSpringNewton);

	
	blinnPhongProperties.diffuseColor = Eigen::Vector3f(0.5f, 1.0f, 0.2f);
	blinnPhongProperties.shininess = 500.0f;
	std::shared_ptr<BlinnPhongMaterial> planeMat = std::make_shared<BlinnPhongMaterial>(blinnPhongShader, blinnPhongProperties);
	std::shared_ptr<TriangularMesh> plane = std::make_shared<TriangularMesh>();
	CustomUtils::importObjModel("resources/Models/plane.obj", false, plane->vertexData.position, plane->vertexData.normal, plane->faceIndices, plane->vertAdjacency);
	plane->generateBuffers();
	plane->transform.scale = Eigen::Vector3f(3, 3, 3);
	this->scene.addSceneObject(plane, planeMat);
	this->engineState.renderState = std::make_shared<RenderState>(scene.renderState);
}


void Engine::start()
{
	for (int i = 0; i < this->scene.sceneObjects.size(); i++)
	{
		if (!this->scene.sceneObjects[i]->isActive)
			continue;
		this->scene.sceneObjects[i]->start(this->engineState);
	}
	glfwGetCursorPos(this->window, &this->engineState.mouse->prevPos.x(), &this->engineState.mouse->prevPos.y());
	this->engineState.mouse->curPos = this->engineState.mouse->prevPos;
	this->engineState.physics->start = std::chrono::high_resolution_clock::now();
	this->engineState.start = std::chrono::high_resolution_clock::now();
}

void Engine::handleEvent(const GLEQevent& event)
{
	this->engineState.keyboard->down.clear();
	this->engineState.keyboard->released.clear();

	switch (event.type)
	{
	case GLEQ_WINDOW_RESIZED:
		this->scene.renderState.windowWidth = event.size.width;
		this->scene.renderState.windowHeight = event.size.height;
		glViewport(0, 0, this->scene.renderState.windowWidth, this->scene.renderState.windowHeight);
		break;
	case GLEQ_KEY_PRESSED:
		this->engineState.keyboard->down.insert(event.keyboard.key);
		this->engineState.keyboard->held.insert(event.keyboard.key);
		this->engineState.keyboard->down.insert(event.keyboard.mods);
		this->engineState.keyboard->held.insert(event.keyboard.mods);
		this->handleInteractions(event.keyboard.key, true);
		break;
	case GLEQ_KEY_RELEASED:
		this->engineState.keyboard->held.erase(event.keyboard.key);
		this->engineState.keyboard->released.insert(event.keyboard.key);
		this->engineState.keyboard->held.erase(event.keyboard.mods);
		this->engineState.keyboard->released.insert(event.keyboard.mods);
		this->handleInteractions(event.keyboard.key, false);
		break;
	case GLEQ_BUTTON_PRESSED:
		this->handleInteractions(event.mouse.button, true);
		switch (event.mouse.button)
		{
		case GLFW_MOUSE_BUTTON_LEFT:
			this->engineState.mouse->isLeftDown = true;
			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			this->engineState.mouse->isRightDown = true;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			this->engineState.mouse->isMiddleDown = true;
			break;
		}
		break;
	case GLEQ_BUTTON_RELEASED:
		this->handleInteractions(event.mouse.button, false);
		switch (event.mouse.button)
		{
		case GLFW_MOUSE_BUTTON_LEFT:
			this->engineState.mouse->isLeftDown = false;
			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			this->engineState.mouse->isRightDown = false;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			this->engineState.mouse->isMiddleDown = false;
			break;
		}
		break;
	case GLEQ_SCROLLED:
		//this->scene.cam->zoom(-event.scroll.y * 1.5f);
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
}

void Engine::update()
{
	this->engineState.deltaTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - this->engineState.prevTime).count();
	this->engineState.mouse->prevPos = this->engineState.mouse->curPos;
	glfwGetCursorPos(this->window, &this->engineState.mouse->curPos.x(), &this->engineState.mouse->curPos.y());
	this->engineState.mouse->deltaPos = this->engineState.mouse->curPos - this->engineState.mouse->prevPos;

	this->handleInteractions();
	this->scene.cam->update(this->engineState);

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
			sceneObj->update(this->engineState);
			if (!sceneObj->isRenderable)
				continue;
			this->scene.sceneObjectMaterialMapping[sceneObj]->use();
			Eigen::Matrix4f modelMatrix = sceneObj->transform.matrix();
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
	std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - this->engineState.physics->start;
	if (elapsed.count() > this->engineState.physics->fixedDeltaTime)
	{
		for (auto& sceneObj : this->scene.sceneObjects)
		{
			if (!sceneObj->isActive)
				continue;
			sceneObj->fixedUpdate(this->engineState);
		}
		this->engineState.physics->start = std::chrono::high_resolution_clock::now();
	}

	this->engineState.prevTime = std::chrono::high_resolution_clock::now();
}

void Engine::stop()
{
	this->scene.clear();
}


