#pragma once

#include "component.h"
#include "gfx.h"
#include "framebuffer.h"

class Light : public Component
{
public:
	LightParameters* lightParameters = nullptr;
	Framebuffer shadowMap;


public:
	Light(LightParameters* _lightParameters, Eigen::Vector2i shadowMapRes) : Component(),lightParameters(_lightParameters)
	{
		if (this->lightParameters->lightType == LightType::Spot)
		{
			this->lightParameters->fov = asinf(CustomUtils::radians(this->lightParameters->outerRadius));
		}
		this->lightParameters->direction = this->lightParameters->target - this->sceneObject->transform.position;
		
		this->shadowMap.createAndAttachTexture(
			Texture::TextureSettings{
				.internalFormat = GL_DEPTH_COMPONENT,
				.width = shadowMapRes.x(),
				.height = shadowMapRes.y(),
				.format = GL_DEPTH_COMPONENT,
				.type = GL_FLOAT,
				.numChannels = 1,
				.magFilter = GL_NEAREST,
				.minFilter = GL_NEAREST,
			}, GL_DEPTH_ATTACHMENT, false);
		//this->lightParameters->shadowMapTextureHandle = 

	}
	Eigen::Matrix4f getLightViewMatrix();
	Eigen::Matrix4f getLightProjectionMatrix();
	void start(const EngineState& engineState) override;
	void update(const EngineState& engineState) override;
};

