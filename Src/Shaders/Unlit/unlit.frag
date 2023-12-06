#version 330 core

layout(location = 0) out vec4 color;
in vec3 diffuseColor;

void main()
{
	color = vec4(diffuseColor,1.0);
}
