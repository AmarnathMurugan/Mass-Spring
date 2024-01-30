#pragma once

#include <Eigen/Dense>
#include <Eigen/Core>
#include <glad/glad.h>
#include <unordered_set>
#include <chrono>
#include "buffer.h"

#define GL_INVALID_INDEX 0xFFFFFFFF


const float  PI_F = 3.14159265358979f;

typedef Eigen::Matrix<float, Eigen::Dynamic, 3,Eigen::RowMajor> MatrixX3fRowMajor;
typedef Eigen::Matrix<double, Eigen::Dynamic, 3, Eigen::RowMajor> MatrixX3dRowMajor;
typedef Eigen::Matrix<uint32_t, Eigen::Dynamic, 3, Eigen::RowMajor> MatrixX3UIRowMajor;
typedef Eigen::Matrix<int, Eigen::Dynamic, 3, Eigen::RowMajor> MatrixX3IRowMajor;
typedef Eigen::Matrix<uint32_t, Eigen::Dynamic, 4, Eigen::RowMajor> MatrixX4UIRowMajor;
typedef Eigen::Matrix<int, Eigen::Dynamic, 4, Eigen::RowMajor> MatrixX4IRowMajor;

struct Transform
{
	Eigen::Vector3f position, scale;
	Eigen::Quaternionf rotation;
	Transform() : position(Eigen::Vector3f::Zero()), scale(Eigen::Vector3f::Ones()), rotation(Eigen::Quaternionf::Identity()) {}

	Eigen::Matrix4f matrix() const
	{
		Eigen::Matrix4f mat = Eigen::Matrix4f::Identity();
		mat.block<3, 3>(0, 0) = rotation.toRotationMatrix();
		mat.block<3, 1>(0, 3) = position;
		mat.block<3, 3>(0, 0) *= scale.asDiagonal();
		return mat;
	}	
};

struct CameraState
{
	Transform* transform = nullptr;
	Eigen::Vector3f lookAtPosition = Eigen::Vector3f::Zero();
	float nearPlane = 0.1, farPlane = 1000, FOV = 45;
	bool isPerspective = true;
};

enum class LightType
{
	Directional,
	Point,
	Spot
};

struct LightParameters
{
	// ========= Common Params ==========
	LightType lightType = LightType::Directional;
	Eigen::Vector3f color = Eigen::Vector3f(1.0f, 1.0f, 1.0f);
	float intensity = 1.0f;
	Eigen::Vector3f direction = Eigen::Vector3f(0.0f, 0.0f, 1.0f);
	bool isShadowCaster = true;

	// ========= Spotlight and point light Params ==========
	float innerRadius = 1.0f;
	float outerRadius = 2.0f;

	// ========= Spotlight & Directional light Params ==========
	Eigen::Vector3f target = Eigen::Vector3f::Zero();
	float fov = 60.0f;
	float nearPlane = 0.1f;
	float farPlane = 100.0f;
	GLuint64 shadowMapTextureHandle = GL_INVALID_INDEX;
};

struct RenderState
{
	Eigen::Vector3f ambientColor = Eigen::Vector3f(1,1,1);
	Eigen::Vector3f clearColor = Eigen::Vector3f(180,230,225)/225.0;
	int windowWidth = 960,windowHeight = 540;
	float ambientIntensity = 0.7f;
    
	CameraState cameraState;
	std::vector<LightParameters> lightStates;

	std::shared_ptr<Buffer> matricesUBO, lightsSSBO;

	
	struct Matrices
	{
		Eigen::Matrix4f viewMatrix, projectionMatrix, VP;
	}globalMatrices;

	

	RenderState()
	{
		matricesUBO = std::make_shared<Buffer>(GL_UNIFORM_BUFFER,GL_STREAM_DRAW,true);
		matricesUBO->initBuffer(0, sizeof(Matrices));
	}

	void setUBOdata()
	{
		matricesUBO->setData(globalMatrices.viewMatrix.data());
	}

	~RenderState()
	{
	}
};

struct KeyboardState
{
	std::unordered_set<int> down, held, released;
};

struct MouseState
{
	bool isLeftDown = false;
	bool isRightDown = false;
	bool isMiddleDown = false;
	float scroll = 0.0f;
	Eigen::Vector2d prevPos, curPos, deltaPos;
};

struct PhysicsSettings
{
	std::chrono::time_point<std::chrono::high_resolution_clock> start;
	double fixedDeltaTime = 1.0 / 30.0;
};

struct EngineState
{
	KeyboardState keyboard;
	MouseState mouse;
	PhysicsSettings physics;
	RenderState* renderState;
	std::chrono::time_point<std::chrono::high_resolution_clock> start;
	std::chrono::time_point<std::chrono::high_resolution_clock> prevTime;
	double deltaTime;
};

struct SurfaceProperties
{
	Eigen::Vector3f diffuseColor = Eigen::Vector3f(1,1,1);
	Eigen::Vector3f specularColor = Eigen::Vector3f(1,1,1);
	float shininess = 32.0f;
};

void GLAPIENTRY openGLErrorMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
