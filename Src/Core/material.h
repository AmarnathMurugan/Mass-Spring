#pragma once

#include "shader.h"
#include "texture.h"

class Material
{
public:
	Material(std::shared_ptr<Shader> _shader);
	virtual void use();
	virtual void setUniforms();
public:
	std::shared_ptr<Shader> shader;
	std::vector<std::shared_ptr<Texture>> textures;
};
