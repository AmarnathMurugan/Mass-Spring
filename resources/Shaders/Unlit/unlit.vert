#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 uVP;
uniform mat4 uModel,uView,uProjection;

out vec3 fragNormal;

void main()
{
	gl_Position = uProjection * uView * uModel * vec4(position, 1.0);
	fragNormal = normal;
}
