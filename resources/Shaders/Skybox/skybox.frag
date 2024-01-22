#version 460 core

layout(location = 0) out vec4 color;

in vec3 envCoords;

layout(binding = 0) uniform samplerCube uCubeMap;


void main()
{
	color = texture(uCubeMap, envCoords);
}
