#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 uMV;
uniform mat4 uProjection;
uniform mat3 uNormalMatrix;


out vec3 fragNormal;
out vec3 viewDir;

void main()
{
	 viewDir = -(uMV * vec4(position,1.0)).xyz;
	 gl_Position = uProjection * vec4(-viewDir, 1.0);
	 fragNormal = uNormalMatrix * normal;
}
