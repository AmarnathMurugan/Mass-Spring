#ifndef BLINN_H
#define BLINN_H
#include<material.h>
#include<glm/glm.hpp>
#include "Components/DirectionalLight.h"

extern std::shared_ptr<DirectionalLight> Light;
extern float ambientIntensity;
extern Eigen::Vector3f ambientColor;

class Blinn : public Material
{
public:
	Blinn(const char* vertShaderPath, const char* fragShaderPath,bool _useAmbientColor, float _shininess, Eigen::Vector3f _diffuseColor, Eigen::Vector3f _specularColor);
	virtual void InitLocations();
	virtual void SetShaderParameters();
	virtual void OnViewChange();
	virtual void OnLightChange();
private:
	Eigen::Vector3f lightDirViewSpace,specularColor;
	GLuint lightDirLoc, lightIntensityLoc, ambientLightColorLoc, specularColorLoc, ambientColorLoc, ambientIntensityLoc, shininessLoc;
	bool useAmbientColor;
	float shininess;
};

Blinn::Blinn(const char* vertShaderPath, const char* fragShaderPath, bool _useAmbientColor = false, float _shininess = 100, Eigen::Vector3f _diffuseColor = Eigen::Vector3f(1.0f, 0.0f, 0.0f), Eigen::Vector3f _specularColor = Eigen::Vector3f(1.0f, 1.0f, 1.0f)):material(vertShaderPath, fragShaderPath, _diffuseColor)
{	
	useAmbientColor = _useAmbientColor;
	specularColor = _specularColor;
	shininess = _shininess;
	InitLocations();
	SetShaderParameters();
}

void Blinn::InitLocations()
{
	lightDirLoc = glGetUniformLocation(program, "_lightDir");
	lightIntensityLoc = glGetUniformLocation(program, "_lightIntensity");
	ambientColorLoc = glGetUniformLocation(program, "_ambientColor");
	ambientIntensityLoc = glGetUniformLocation(program, "_ambientIntensity");
	specularColorLoc = glGetUniformLocation(program, "_specularColor");
	shininessLoc = glGetUniformLocation(program, "_shininess");
}

void Blinn::SetShaderParameters()
{
	material::SetShaderParameters();
	if (Light == NULL)
		std::cerr << "No light found in scene";
	lightDirViewSpace = Cam->getViewMatrix() * glm::vec4(glm::normalize(Light->sceneObject->position),0.0f);
	glUniform3f(lightDirLoc, lightDirViewSpace.x, lightDirViewSpace.y, lightDirViewSpace.z);
	glUniform3f(specularColorLoc, specularColor.r, specularColor.b, specularColor.b);
	glUniform1f(lightIntensityLoc, Light->intensity);
	glUniform1f(ambientIntensityLoc, ambientIntensity);
	glUniform1f(shininessLoc, shininess);
	if(useAmbientColor)
		glUniform3f(ambientColorLoc, ambientColor.r, ambientColor.g, ambientColor.b);
	else
	    glUniform3f(ambientColorLoc, diffuseColor.r, diffuseColor.g, diffuseColor.b);
}

void Blinn::OnViewChange()
{
	Use();
	lightDirViewSpace = Cam->getViewMatrix() * glm::vec4(glm::normalize(Light->sceneObject->position), 0.0f);
	glUniform3f(lightDirLoc, lightDirViewSpace.x, lightDirViewSpace.y, lightDirViewSpace.z);
}

void Blinn::OnLightChange()
{
	OnViewChange();
	glUniform1f(lightIntensityLoc, Light->intensity);
}

#endif // !BLINN_H

