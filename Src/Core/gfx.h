#pragma once

#include <Eigen/Dense>
#include <Eigen/Core>
#include <glad/glad.h>

#define GL_INVALID_INDEX 0xFFFFFFFF


const float  PI_F = 3.14159265358979f;


typedef Eigen::Transform<float, 3, Eigen::Affine> Transform;
typedef Eigen::Matrix<float, Eigen::Dynamic, 3,Eigen::RowMajor> MatrixX3fRowMajor;
typedef Eigen::Matrix<double, Eigen::Dynamic, 3, Eigen::RowMajor> MatrixX3dRowMajor;
typedef Eigen::Matrix<uint32_t, Eigen::Dynamic, 3, Eigen::RowMajor> MatrixX3UIRowMajor;
typedef Eigen::Matrix<int, Eigen::Dynamic, 3, Eigen::RowMajor> MatrixX3IRowMajor;
typedef Eigen::Matrix<uint32_t, Eigen::Dynamic, 4, Eigen::RowMajor> MatrixX4UIRowMajor;
typedef Eigen::Matrix<int, Eigen::Dynamic, 4, Eigen::RowMajor> MatrixX4IRowMajor;

struct RenderState
{
	Eigen::Matrix4f viewMatrix,projectionMatrix;
	Eigen::Vector3f cameraPosition;
	Eigen::Vector3f ambientColor = Eigen::Vector3f(1,1,1);
	Eigen::Vector3f clearColor = Eigen::Vector3f(180,230,225)/225.0;
	int windowWidth = 960,windowHeight = 540;
	float ambientIntensity = 0.7f;

	Eigen::Vector3f lightDir = Eigen::Vector3f(1,1,0);
	Eigen::Vector3f lightColor = Eigen::Vector3f(1,1,1);
	float lightIntensity = 1.0f;

};

struct SurfaceProperties
{
	Eigen::Vector3f diffuseColor = Eigen::Vector3f(1,1,1);
	Eigen::Vector3f specularColor = Eigen::Vector3f(1,1,1);
	float shininess = 32.0f;
};

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
