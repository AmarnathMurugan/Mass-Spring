#version 460 core

layout(location = 0) in vec3 position;

uniform mat4 uVP;
uniform mat4 uModel,uView,uProjection;

void main()
{
	gl_Position = uVP * uModel * vec4(position, 1.0);
}
