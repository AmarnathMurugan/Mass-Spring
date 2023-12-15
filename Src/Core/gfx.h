#pragma once

#include <Eigen/Dense>
#include <Eigen/Core>
#include <glad/glad.h>
#include <unordered_set>
#include <chrono>

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

	Eigen::Vector3f lightDir = Eigen::Vector3f(0,1,1);
	Eigen::Vector3f lightColor = Eigen::Vector3f(1,1,1);
	float lightIntensity = 1.0f;

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
	Eigen::Vector2d prevPos, curPos, deltaPos;
};

struct PhysicsSettings
{
	std::chrono::time_point<std::chrono::high_resolution_clock> start;
	double fixedDeltaTime = 1.0 / 60.0;
};

struct EngineState
{
	std::shared_ptr<KeyboardState> keyboard;
	std::shared_ptr<MouseState> mouse;
	std::shared_ptr<PhysicsSettings> physics;
	std::shared_ptr<RenderState> renderState;
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

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
