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
	shader = glCreateShader((GLenum)shaderType)  ;
	glShaderSource(shader, 1, &shaderContent, NULL) ;
	glCompileShader(shader) ;
	int success;
	char log[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success) ;
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, log) ;
		std::string shaderName = "";
		std::cout << shaderName << " shader compilation failed\n" << log << std::endl;
		return -1;
	}
	return shader;
}

void Shader::linkShaders()
{
	program = glCreateProgram() ;
	for(auto& shader:shaders)
		glAttachShader(program, shader.second) ;
	glLinkProgram(program) ;
	int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success) ;
	char log[512];
	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, log) ;
		std::cout << "Linking Failed:" << log << std::endl;
	}
	std::cerr << "Shader Compilation Complete \n";
	for (auto& shader : shaders)
		glDeleteShader(shader.second) ;
}


void Shader::recompileShaders()
{
	if (program != 0)
	{
		glDeleteProgram(program) ;
	}
	for (auto& shader : shaderPaths)
	{
		shaders[shader.first] = compileShader(shader.second, shader.first);
	}
	linkShaders();
}


void Shader::bind()
{
	glUseProgram(program) ;

}

inline GLuint Shader::getProgram() const
{
	return program;
}

GLuint Shader::getUniformLocation(std::string name)
{
	if(uniformLocations.find(name) == uniformLocations.end())
		uniformLocations[name] = glGetUniformLocation(program, name.c_str()) ;
	return uniformLocations[name];
}

void Shader::setUniform(std::string name, bool value)
{
	glUniform1ui(this->getUniformLocation(name), value) ;
}

void Shader::setUniform(std::string name, GLuint value)
{
	glUniform1ui(this->getUniformLocation(name), value) ;
}

void Shader::setUniform(std::string name, GLint value)
{
	glUniform1i(this->getUniformLocation(name), value) ;
}

void Shader::setUniform(std::string name, GLfloat value)
{
	glUniform1f(this->getUniformLocation(name), value) ;
}

void Shader::setUniform(std::string name, const Eigen::Vector2i& value)
{
	glUniform2i(this->getUniformLocation(name), value.x(), value.y()) ;
}

void Shader::setUniform(std::string name, const Eigen::Vector2f& value)
{
	glUniform2f(this->getUniformLocation(name), value.x(), value.y()) ;
}

void Shader::setUniform(std::string name, const Eigen::Vector3f& value)
{
	glUniform3f(this->getUniformLocation(name), value.x(), value.y(), value.z()) ;
}

void Shader::setUniform(std::string name, const Eigen::Vector4f& value)
{
	glUniform4f(this->getUniformLocation(name), value.x(), value.y(), value.z(), value.w()) ;
}

void Shader::setUniform(std::string name, const Eigen::Matrix4f& value)
{
	glUniformMatrix4fv(this->getUniformLocation(name), 1, GL_FALSE, value.data()) ;
}