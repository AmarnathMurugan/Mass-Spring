#pragma once
#include "material.h"

class SkyboxMaterial : public Material
{
public:
	void setUniforms() override;
	SkyboxMaterial(const std::shared_ptr<Shader>& _shader, const Eigen::Vector3f& _diffuse);
private:
	Eigen::Vector3f diffuseColor = Eigen::Vector3f(1.0f, 1.0f, 1.0f);
};

