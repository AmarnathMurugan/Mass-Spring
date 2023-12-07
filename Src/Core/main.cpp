#define NOMINMAX

#include "includes.h"
#include "camera.h"
#include "triangularMesh.h"
#include "unlitMaterial.h"
//#include "materials/Blinn.h"
//#include "InteractionHandler.h"
//#include "Components/DirectionalLight.h"

#pragma region Global Variables
std::vector<std::shared_ptr<SceneObject>> sceneObjects;
std::vector<std::shared_ptr<Shader>> shaders;
std::unordered_map<std::shared_ptr<SceneObject>, std::shared_ptr<Material>> sceneObjectMaterialMapping;
std::shared_ptr<Camera> cam;
RenderState renderState;
#pragma endregion

void createWorld();

int main(int argc, char* argv[])
{
	// Setup GLFW
	if (!glfwInit())
	{
		std::cerr << "\nUnable to initialize GLFW";
		return -1;
	}	

	// Setup window
	GLFWwindow* window = glfwCreateWindow(renderState.windowWidth, renderState.windowHeight, "Mass-Spring ADMM", NULL, NULL);
	if (!window)
	{
		std::cerr<<"\nUnable to create window";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Setup GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "\nUnable to initialize GLAD";
		return -1;
	}

	// Initialize interaction callbacks
	//InitInteractionHandlers(window);
	

	// Init OpenGL Parameters
	glClearColor(renderState.clearColor.x(), renderState.clearColor.y(), renderState.clearColor.z(), 1.0f) $GL_CATCH_ERROR;
	glViewport(0, 0, renderState.windowWidth, renderState.windowHeight) $GL_CATCH_ERROR;
	glEnable(GL_DEPTH_TEST) $GL_CATCH_ERROR;

	// Init World
	createWorld();

	for (int i = 0; i < sceneObjects.size(); i++)
	{
		if (!sceneObjects[i]->isActive)
			continue;
		sceneObjects[i]->Start();
	}

	// Render Loop
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) $GL_CATCH_ERROR;
		
        glfwPollEvents();

		// iterate through all shaders and set transformation matrices
		for (int j = 0; j < shaders.size(); j++)
		{
			shaders[j]->bind();
			Eigen::Matrix4f viewMatrix = cam->viewMatrix();
			Eigen::Matrix4f projectionMatrix = cam->projectionMatrix(renderState.windowWidth, renderState.windowHeight);
			Eigen::Matrix4f VP = projectionMatrix * viewMatrix;
			shaders[j]->setUniform("uView", viewMatrix);
			shaders[j]->setUniform("uProjection", projectionMatrix);
			shaders[j]->setUniform("uVP", VP);
		}

		for (int i = 0; i < sceneObjects.size(); i++)	
		{
			if (!sceneObjects[i]->isActive)
				continue;
            sceneObjects[i]->Update();
			if (!sceneObjects[i]->isRenderable)
				continue;
			sceneObjectMaterialMapping[sceneObjects[i]]->use();
			sceneObjectMaterialMapping[sceneObjects[i]]->shader->setUniform("uModel",sceneObjects[i]->getModelMatrix());
            sceneObjects[i]->render();
		}
		
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}


void createWorld()
{
	cam = std::make_shared<Camera>(Eigen::Vector3f::Zero(), 50);
	std::shared_ptr<Shader> unlitShader = std::make_shared<Shader>(
		std::unordered_map<ShaderType, std::string>
		{
			{ShaderType::VertexShader,"resources/Shaders/Unlit/unlit.vert"},
			{ShaderType::FragmentShader,"resources/Shaders/Unlit/unlit.frag"}
		});

	shaders.emplace_back(unlitShader);
	std::shared_ptr<UnlitMaterial> unlitMaterial = std::make_shared<UnlitMaterial>(unlitShader,Eigen::Vector3f(255.f,255.1f,255.0f));
	 
	std::shared_ptr<TriangularMesh> mesh = std::make_shared<TriangularMesh>();
	sceneObjects.emplace_back(mesh);
	sceneObjectMaterialMapping[mesh] = unlitMaterial;

	// Create quad with screen-space coordinates center of screen
	mesh->vertexData.position.resize(4, 3);
	mesh->vertexData.position << -0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.5f,  0.5f, 0.0f,
	   -0.5f,  0.5f, 0.0f;
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
