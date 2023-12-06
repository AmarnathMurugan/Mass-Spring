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
	shader = glCreateShader((GLenum)shaderType);
	glShaderSource(shader, 1, &shaderContent, NULL);
	glCompileShader(shader);
	int success;
	char log[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, log);
		std::string shaderName = "";
		std::cout << shaderName << " shader compilation failed\n" << log << std::endl;
		return -1;
	}
	return shader;
}

void Material::linkShaders()
{
	program = glCreateProgram();
	for(auto& shader:shaders)
		glAttachShader(program, shader.second);
	glLinkProgram(program);
	int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	char log[512];
	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, log);
		std::cout << "Linking Failed:" << log << std::endl;
	}
	std::cerr << "Shader Compilation Complete \n";
	for (auto& shader : shaders)
		glDeleteShader(shader.second);
}


void Material::recompileShaders()
{
	glDeleteProgram(program);
	for (auto& shader : shaderPaths)
	{
		shaders[shader.first] = compileShader(shader.second, shader.first);
	}
	linkShaders();
	setShaderParameters();
}


void Material::use()
{
	glUseProgram(program);
}

inline GLuint Material::getProgram() const
{
	return program;
}
