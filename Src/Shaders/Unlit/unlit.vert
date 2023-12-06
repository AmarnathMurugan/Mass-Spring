#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 MVP;
uniform vec3 _diffuseColor;

out vec3 diffuseColor;

void main()
{
	diffuseColor = _diffuseColor;
	gl_Position = MVP * vec4(position, 1.0);
}
