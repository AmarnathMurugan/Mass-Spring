#pragma once

#include "includes.h"

enum class ShaderType
{
	VertexShader = GL_VERTEX_SHADER,
	FragmentShader = GL_FRAGMENT_SHADER,
	GeometryShader = GL_GEOMETRY_SHADER,
	TessellationControlShader = GL_TESS_CONTROL_SHADER,
	TessellationEvaluationShader = GL_TESS_EVALUATION_SHADER
};

class Shader
{
public:
	Shader(const std::unordered_map<ShaderType,std::string>& _shaderPaths);
	void bind();
	void recompileShaders();
	GLuint getProgram() const;
    GLuint getUniformLocation(std::string name);


    // Set bool uniform
    void setUniform(std::string name, bool value);
    // Set unsigned integer uniform
    void setUniform(std::string name, GLuint value);
    // Set integer uniform
    void setUniform(std::string name, GLint value);
    // Set float uniform
    void setUniform(std::string name, GLfloat value);
    // Set vec2i uniform
    void setUniform(std::string name, const Eigen::Vector2i& value);
    // Set vec2 uniform
    void setUniform(std::string name, const Eigen::Vector2f& value);
    // Set vec3 uniform
    void setUniform(std::string name, const Eigen::Vector3f& value);
    // Set vec4 uniform
    void setUniform(std::string name, const Eigen::Vector4f& value);
    // Set mat4 uniform
    void setUniform(std::string name, const Eigen::Matrix4f& value);

private:
	GLuint compileShader(std::string pth, ShaderType shaderType);
	void linkShaders();
	
protected:
	GLuint program = 0;
	std::unordered_map<ShaderType, std::string> shaderPaths;
	std::unordered_map<ShaderType, GLuint> shaders;
	std::unordered_map<std::string, GLuint> uniformLocations;
};
