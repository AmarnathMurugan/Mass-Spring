#version 460 core

layout(location = 0) in vec3 position;

uniform mat4 viewProjInv;

out vec3 envCoords;

void main()
{
	envCoords = (viewProjInv * vec4(position, 1.0)).xyz;
	gl_Position = vec4(position, 1.0);
}
