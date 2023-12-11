#define NOMINMAX

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLEQ_IMPLEMENTATION
#include "gleq.h"
#include "engine.h"

int main(int argc, char* argv[])
{
	// Setup GLFW
	if (!glfwInit())
	{
		std::cerr << "\nUnable to initialize GLFW";
		return -1;
	}

	gleqInit();

	// Setup window
	GLFWwindow* window = glfwCreateWindow(960, 540, "Mass-Spring ADMM", NULL, NULL);
	if (!window)
	{
		std::cerr<<"\nUnable to create window";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	gleqTrackWindow(window);

	// Setup GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "\nUnable to initialize GLAD";
		return -1;
	}
	std::string versionString = std::string((const char*)glGetString(GL_VERSION));

	// Setup engine
	Engine engine(window);
	engine.start();

	GLEQevent event;
	// Core Loop
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) ;
		
        glfwPollEvents();
		while (gleqNextEvent(&event))
		{
			engine.handleEvent(event);
			gleqFreeEvent(&event);
		}

		engine.update();
		
		glfwSwapBuffers(window);
	}

	engine.stop();
	glfwTerminate(); 
	return 0;
}