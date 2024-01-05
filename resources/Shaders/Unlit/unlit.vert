#version 460 core

layout(location = 0) in vec3 position;

layout (std140, binding = 0) uniform Matrices
{
	mat4 uView;
	mat4 uProjection;
	mat4 uVP;
};

uniform mat4 uModel;

void main()
{
	gl_Position = uVP * uModel * vec4(position, 1.0);
}
