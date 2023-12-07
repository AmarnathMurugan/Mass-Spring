#include "shader.h"

Shader::Shader(const std::unordered_map<ShaderType, std::string>& _shaderPaths):
	shaderPaths(_shaderPaths)
{
	recompileShaders();
}


GLuint Shader::compileShader(std::string pth, ShaderType shaderType)
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

void Shader::linkShaders()
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


void Shader::recompileShaders()
{
	if (program != 0)
	{
		glDeleteProgram(program) $GL_CATCH_ERROR;
	}
	for (auto& shader : shaderPaths)
	{
		shaders[shader.first] = compileShader(shader.second, shader.first);
	}
	linkShaders();
}


void Shader::bind()
{
	glUseProgram(program) $GL_CATCH_ERROR;

}

inline GLuint Shader::getProgram() const
{
	return program;
}

GLuint Shader::getUniformLocation(std::string name)
{
	if(uniformLocations.find(name) == uniformLocations.end())
		uniformLocations[name] = glGetUniformLocation(program, name.c_str()) $GL_CATCH_ERROR;
	return uniformLocations[name];
}

void Shader::setUniform(std::string name, bool value)
{
	glUniform1ui(this->getUniformLocation(name), value) $GL_CATCH_ERROR;
}

void Shader::setUniform(std::string name, GLuint value)
{
	glUniform1ui(this->getUniformLocation(name), value) $GL_CATCH_ERROR;
}

void Shader::setUniform(std::string name, GLint value)
{
	glUniform1i(this->getUniformLocation(name), value) $GL_CATCH_ERROR;
}

void Shader::setUniform(std::string name, GLfloat value)
{
	glUniform1f(this->getUniformLocation(name), value) $GL_CATCH_ERROR;
}

void Shader::setUniform(std::string name, const Eigen::Vector2i& value)
{
	glUniform2i(this->getUniformLocation(name), value.x(), value.y()) $GL_CATCH_ERROR;
}

void Shader::setUniform(std::string name, const Eigen::Vector2f& value)
{
	glUniform2f(this->getUniformLocation(name), value.x(), value.y()) $GL_CATCH_ERROR;
}

void Shader::setUniform(std::string name, const Eigen::Vector3f& value)
{
	glUniform3f(this->getUniformLocation(name), value.x(), value.y(), value.z()) $GL_CATCH_ERROR;
}

void Shader::setUniform(std::string name, const Eigen::Vector4f& value)
{
	glUniform4f(this->getUniformLocation(name), value.x(), value.y(), value.z(), value.w()) $GL_CATCH_ERROR;
}

void Shader::setUniform(std::string name, const Eigen::Matrix4f& value)
{
	glUniformMatrix4fv(this->getUniformLocation(name), 1, GL_FALSE, value.data()) $GL_CATCH_ERROR;
}