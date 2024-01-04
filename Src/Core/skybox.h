#pragma once
#include "sceneObject.h"

class Skybox : public SceneObject
{
	public:
		Skybox();
		void setBuffers();
		void render() override;

	private:
		GLuint VAO = GL_INVALID_INDEX, VBO = GL_INVALID_INDEX;
		
		std::vector<float> vertData{ -1.0f,-1.0f,0.999f,
									-1.0f,4.0f,0.999f,
									4.0f,-1.0f,0.999f };
};
