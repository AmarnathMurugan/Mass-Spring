#include "blinnPhongMaterial.h"


BlinnPhongMaterial::BlinnPhongMaterial(std::shared_ptr<Shader> _shader, SurfaceProperties _properties) : Material(_shader), surfaceProperties(_properties)
{
}

void BlinnPhongMaterial::setUniforms()
{
	this->shader->setUniform("uDiffuseColor", this->surfaceProperties.diffuseColor);
	this->shader->setUniform("uSpecularColor", this->surfaceProperties.specularColor);
	this->shader->setUniform("uShininess", this->surfaceProperties.shininess);
}
