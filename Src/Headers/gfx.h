#pragma once
#include <Eigen/Dense>
#include <Eigen/Core>
#include <glad/glad.h>

#define GL_INVALID_INDEX -1


const float  PI_F = 3.14159265358979f;
Eigen::Vector3f ORIGIN = Eigen::Vector3f::Zero();
Eigen::Vector3f UP = Eigen::Vector3f(0, 1, 0);
Eigen::Vector3f RIGHT = Eigen::Vector3f(1, 0, 0);
Eigen::Vector3f FORWARD = Eigen::Vector3f(0, 0, 1);
Eigen::Vector3f LEFT = Eigen::Vector3f(-1, 0, 0);
Eigen::Vector3f DOWN = Eigen::Vector3f(0, -1, 0);
Eigen::Vector3f BACK = Eigen::Vector3f(0, 0, -1);


typedef Eigen::Transform<float, 3, Eigen::Affine> Transform;
typedef Eigen::Matrix<float, Eigen::Dynamic, 3,Eigen::RowMajor> MatrixX3fRowMajor;
typedef Eigen::Matrix<uint32_t, Eigen::Dynamic, 3, Eigen::RowMajor> MatrixX3UIRowMajor;

void glCatchError(const char* file, int line);
#ifdef _DEBUG
	#define $GL_CATCH_ERROR ;glCatchError(__FILE__,__LINE__);
#else
	#define $GL_CATCH_ERROR ;((void)0);
#endif

struct RenderState
{
	Eigen::Matrix4f viewMatrix,projectionMatrix;
	Eigen::Vector3f cameraPosition;
	Eigen::Vector3f ambientColor = Eigen::Vector3f(1,1,1);
	Eigen::Vector3f clearColor = Eigen::Vector3f(0,0,0);
	int windowWidth = 960,windowHeight = 540;
	float ambientIntensity = 0.2f;
};

