#include "material.h"

Material::Material(std::shared_ptr<Shader> shader) : shader(shader) {}

void Material::use() {
	shader->bind();
	setUniforms();
}

void Material::setUniforms()
{
	for (auto& texture: textures)
	{
		texture->bind();
	}
}