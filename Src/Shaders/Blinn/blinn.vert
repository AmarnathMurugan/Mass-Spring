#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 MVP;
uniform mat4 MV;
uniform mat3 NormalMatrix;
uniform vec3 _diffuseColor;
uniform vec3 _specularColor;
uniform vec3 _ambientColor;
uniform vec3 _lightDir;
uniform float _lightIntensity;
uniform float _ambientIntensity;
uniform float _shininess;

out vec3 diffuseColor;
out vec3 specularColor;
out vec3 viewSpaceNormal;
out vec3 lightDir;
out vec3 ambientColor;
out vec3 viewDir;
out float lightIntensity;
out float ambientIntensity;
out float shininess;
void main()
{
	diffuseColor = _diffuseColor;
	specularColor = _specularColor;
	viewSpaceNormal = NormalMatrix*normal;
	lightDir = _lightDir;
	ambientColor = _ambientColor;
	lightIntensity = _lightIntensity;
	ambientIntensity = _ambientIntensity;
	shininess = _shininess;
	viewDir = -normalize((MV* vec4(position, 1.0)).xyz);
	gl_Position = MVP * vec4(position, 1.0);
	
}
