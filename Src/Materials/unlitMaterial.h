#pragma once

#include "material.h"

class UnlitMaterial : public Material
{
public:
	Eigen::Vector3f diffuseColor = Eigen::Vector3f(1.0f, 1.0f, 1.0f);

public:
	UnlitMaterial(const std::shared_ptr<Shader>& _shader, const Eigen::Vector3f& _diffuse);
	virtual void setUniforms() override;

};
