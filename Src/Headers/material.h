#ifndef MATERIAL_H
#define MATERIAL_H

#include "includes.h"

enum class ShaderType
{
	VertexShader = GL_VERTEX_SHADER,
	FragmentShader = GL_FRAGMENT_SHADER,
	GeometryShader = GL_GEOMETRY_SHADER,
	TessellationControlShader = GL_TESS_CONTROL_SHADER,
	TessellationEvaluationShader = GL_TESS_EVALUATION_SHADER
};

class Material
{
public:
	Material(std::unordered_map<ShaderType,const char*> _shaderPaths, Eigen::Vector3f _diffuseColor);
	void use();
	void recompileShaders();
	virtual void setShaderParameters() = 0;
	GLuint getProgram() const;

private:
	GLuint compileShader(const char* pth, ShaderType shaderType);
	void linkShaders();
	
protected:
	GLuint program;
	std::unordered_map<ShaderType, const char*> shaderPaths;
	std::unordered_map<ShaderType, GLuint> shaders;
};


#endif // !MATERIAL_H

