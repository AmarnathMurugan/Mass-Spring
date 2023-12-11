#version 460 core

layout(location = 0) out vec4 color;

in vec3 fragNormal;
in vec3 viewDir;

uniform vec3 uViewLightDir, uLightColor;
uniform vec3 uDiffuseColor;
uniform vec3 uSpecularColor;
uniform vec3 uAmbientColor;

uniform float uShininess;
uniform float uAmbientIntensity;
uniform float uLightIntensity;

void main()
{
	vec3 viewDirNorm = normalize(viewDir);
	vec3 normal = normalize(fragNormal);
	vec3 curColor = uDiffuseColor * max(dot(normal, uViewLightDir), 0.0f);
	vec3 reflectDir = reflect(-uViewLightDir, normal);
	curColor += uSpecularColor * pow(max(dot(reflectDir, viewDirNorm), 0.0f), uShininess);
	curColor += uDiffuseColor * uAmbientColor * uAmbientIntensity;
	curColor *= uLightIntensity * uLightColor;
	color = vec4(curColor, 1.0f);
}
