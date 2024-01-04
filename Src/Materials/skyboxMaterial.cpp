#include "skyboxMaterial.h"


SkyboxMaterial::SkyboxMaterial(const std::shared_ptr<Shader>& _shader, const Eigen::Vector3f& _diffuse) : Material(_shader), diffuseColor(_diffuse)
{
}

void SkyboxMaterial::setUniforms()
{
	this->shader->setUniform("uDiffuseColor", diffuseColor);
}