#ifndef SHADER_H
#define SHADER_H

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
	Shader(std::unordered_map<ShaderType,const char*> _shaderPaths);
	void use();
	void recompileShaders();
	GLuint getProgram() const;

private:
	GLuint compileShader(const char* pth, ShaderType shaderType);
	void linkShaders();
	
protected:
	GLuint program;
	std::unordered_map<ShaderType, const char*> shaderPaths;
	std::unordered_map<ShaderType, GLuint> shaders;
	std::unordered_map<std::string, GLuint> uniformLocations;
};


#endif // !SHADER_H

