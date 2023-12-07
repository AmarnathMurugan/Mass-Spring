#pragma once

#include "shader.h"

class Material
{
public:
	Material(std::shared_ptr<Shader> _shader);
	virtual void use();
	virtual void setUniforms() = 0;
public:
	std::shared_ptr<Shader> shader;
};
