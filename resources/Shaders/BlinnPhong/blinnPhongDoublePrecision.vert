#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

layout (std140, binding = 0) uniform Matrices
{
	mat4 uView;
	mat4 uProjection;
	mat4 uVP;
};

uniform mat4 uMV;
uniform mat3 uNormalMatrix;


out vec3 fragNormal;
out vec3 viewDir;

void main()
{
	 viewDir = -(uMV * vec4(position,1.0)).xyz;
	 gl_Position = uProjection * vec4(-viewDir, 1.0);
	 fragNormal = uNormalMatrix * normal;
}
