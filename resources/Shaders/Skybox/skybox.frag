#version 460 core

layout(location = 0) out vec4 color;

uniform vec3 uDiffuseColor;

layout(binding = 0) uniform samplerCube uCubeMap;

void main()
{
	color = vec4(uDiffuseColor,1.0);
}
