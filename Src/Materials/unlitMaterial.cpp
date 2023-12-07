#include "unlitMaterial.h"

UnlitMaterial::UnlitMaterial(const std::shared_ptr<Shader>& _shader, const Eigen::Vector3f& _diffuse): Material(_shader)
	, diffuseColor(_diffuse)
{
}

void UnlitMaterial::setUniforms()
{
	this->shader->setUniform("uDiffuseColor", diffuseColor);
}