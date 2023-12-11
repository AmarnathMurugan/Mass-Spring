#pragma once

#include "Material.h"

class BlinnPhongMaterial : public Material
{
public:
	BlinnPhongMaterial(std::shared_ptr<Shader> _shader, SurfaceProperties _properties = SurfaceProperties());
	virtual void setUniforms() override;
public:
	SurfaceProperties surfaceProperties;
};
