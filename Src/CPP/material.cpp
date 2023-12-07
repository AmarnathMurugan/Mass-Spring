#include "material.h"

Material::Material(std::unordered_map<ShaderType, const char*> _shaderPaths, Eigen::Vector3f _diffuseColor):
	shaderPaths(_shaderPaths)
{
	recompileShaders();
}


GLuint Material::compileShader(const char* pth, ShaderType shaderType)
{
	std::string fileContent = GetStringFromFile(pth);
	if (fileContent == "\0")
		std::cerr << "\n File not found";
	const char* shaderContent = fileContent.c_str();
	GLuint shader;
	shader = glCreateShader((GLenum)shaderType)  $GL_CATCH_ERROR;
	glShaderSource(shader, 1, &shaderContent, NULL) $GL_CATCH_ERROR;
	glCompileShader(shader) $GL_CATCH_ERROR;
	int success;
	char log[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success) $GL_CATCH_ERROR;
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, log) $GL_CATCH_ERROR;
		std::string shaderName = "";
		std::cout << shaderName << " shader compilation failed\n" << log << std::endl;
		return -1;
	}
	return shader;
}

void Material::linkShaders()
{
	program = glCreateProgram() $GL_CATCH_ERROR;
	for(auto& shader:shaders)
		glAttachShader(program, shader.second) $GL_CATCH_ERROR;
	glLinkProgram(program) $GL_CATCH_ERROR;
	int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success) $GL_CATCH_ERROR;
	char log[512];
	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, log) $GL_CATCH_ERROR;
		std::cout << "Linking Failed:" << log << std::endl;
	}
	std::cerr << "Shader Compilation Complete \n";
	for (auto& shader : shaders)
		glDeleteShader(shader.second) $GL_CATCH_ERROR;
}


void Material::recompileShaders()
{
	glDeleteProgram(program) $GL_CATCH_ERROR;
	for (auto& shader : shaderPaths)
	{
		shaders[shader.first] = compileShader(shader.second, shader.first);
	}
	linkShaders();
	setShaderParameters();
}


void Material::use()
{
	glUseProgram(program) $GL_CATCH_ERROR;
}

inline GLuint Material::getProgram() const
{
	return program;
}
