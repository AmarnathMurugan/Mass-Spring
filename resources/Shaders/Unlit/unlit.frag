#version 330 core

layout(location = 0) out vec4 color;

uniform vec3 uDiffuseColor;

in vec3 fragNormal;

void main()
{
	color = vec4(mix(uDiffuseColor,fragNormal,1-0.99),1.0);
}
