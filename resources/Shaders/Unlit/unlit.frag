#version 330 core

layout(location = 0) out vec4 color;
in vec3 uDiffuseColor;

void main()
{
	color = vec4(uDiffuseColor,1.0);
}
