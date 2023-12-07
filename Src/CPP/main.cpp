#define NOMINMAX

#include "includes.h"
#include "triangularMesh.h"
#include "materials/Blinn.h"
#include "InteractionHandler.h"
#include "Components/DirectionalLight.h"

#pragma region Global Variables

std::vector<std::shared_ptr<SceneObject>> World;
std::vector<std::shared_ptr<Material>> materials;
std::shared_ptr<DirectionalLight> Light;
std::shared_ptr<Camera> Cam;

RenderState renderState;
#pragma endregion

void CreateWorld(const char* argv);

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
	InitInteractionHandlers(window);
	

	// Init OpenGL Parameters
	glClearColor(renderState.clearColor.x(), renderState.clearColor.y(), renderState.clearColor.z(), 1.0f) $GL_CATCH_ERROR;
	glViewport(0, 0, renderState.windowWidth, renderState.windowHeight) $GL_CATCH_ERROR;
	glEnable(GL_DEPTH_TEST) $GL_CATCH_ERROR;
	// Init World
	CreateWorld(argv[1]);

	for (int i = 0; i < World.size(); i++)
	{
		if (!World[i]->isActive)
			continue;
		World[i]->Start();
	}

	// Render Loop
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) $GL_CATCH_ERROR;
		
        glfwPollEvents();

		for (int i = 0; i < World.size(); i++)	
		{
			if (!World[i]->isActive)
				continue;
            World[i]->Update();
			if (!World[i]->isRenderable)
				continue;
            World[i]->Render();
		}
		
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}


void CreateWorld(const char* argv)
{
	Cam = std::make_shared<Camera>(Eigen::Vector3f(0.0), 50);

	auto mat = std::make_shared<material>("Src/Shaders/Unlit/unlit.vert", "Src/Shaders/Unlit/unlit.frag",Eigen::Vector3f(1));
	materials.emplace_back(mat);
	auto sphere = std::make_shared <ObjModel>("./Src/Models/sphere.obj", false, mat);
	sphere->transform.translate(Eigen::Vector3f(0, 20, 0));
	sphere->transform.scale(Eigen::Vector3f(0.5, 0.5, 0.5));
	sphere->UpdateMVP();
	Light = std::make_shared<DirectionalLight>();
	sphere->AddComponent(Light);
	World.emplace_back(sphere);

	auto plane = std::make_shared<ObjModel>("./Src/Models/Plane.obj", false, mat);
	plane->transform.translate(Eigen::Vector3f(0, -1, 0));
	plane->transform.scale(Eigen::Vector3f(10, 10, 10));
	//plane->UpdateMVP();	
	World.emplace_back(plane);


	mat = std::make_shared<Blinn>("Src/Shaders/Blinn/blinn.vert", "Src/Shaders/Blinn/blinn.frag");
	materials.emplace_back(mat);

	auto model = std::make_shared <ObjModel>("./Src/Models/Teapot/teapot.obj", false, mat);
	model->name = "Teapot";
	//model->model.ComputeBoundingBox();
	//if (model->model.IsBoundBoxReady()) //TODO: POSITION INCORRECT
	//{
	//	cy::Vec3f center = (model->model.GetBoundMin() + model->model.GetBoundMax()) / 2.0f;
	//	model->position = -1.0f * Eigen::Vector3f(center.x, center.y, center.z);
	//}
	model->transform.rotate(Eigen::AngleAxisf(-90.0f * PI_F,Eigen::Vector3f::UnitX()));
	World.emplace_back(model);

	
	
}
