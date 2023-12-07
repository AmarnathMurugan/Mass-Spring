#pragma once

#include <Eigen/Dense>
#include <Eigen/Core>
#include <glad/glad.h>

#define GL_INVALID_INDEX -1


const float  PI_F = 3.14159265358979f;


typedef Eigen::Transform<float, 3, Eigen::Affine> Transform;
typedef Eigen::Matrix<float, Eigen::Dynamic, 3,Eigen::RowMajor> MatrixX3fRowMajor;
typedef Eigen::Matrix<uint32_t, Eigen::Dynamic, 3, Eigen::RowMajor> MatrixX3UIRowMajor;

struct RenderState
{
	Eigen::Matrix4f viewMatrix,projectionMatrix;
	Eigen::Vector3f cameraPosition;
	Eigen::Vector3f ambientColor = Eigen::Vector3f(1,1,1);
	Eigen::Vector3f clearColor = Eigen::Vector3f(0,0,0);
	int windowWidth = 960,windowHeight = 540;
	float ambientIntensity = 0.2f;
};

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
