#pragma once

#include <glad/glad.h>
#include <string>
#include <vector>
#include <filesystem>
#include <iostream>
#include <stb/stb_image.h>
#include <Eigen/Core>


class Texture
{
public:
	struct TextureSettings
	{
		GLuint texUnit = 0;
		GLenum target = GL_TEXTURE_2D;		
		GLint level = 0;
		GLint internalFormat = GL_RGBA;
		GLsizei width = 0;
		GLsizei height = 0;
		GLint border = 0;
		GLenum format = GL_RGBA;
		GLenum type = GL_UNSIGNED_BYTE;
		int numChannels = 4;
		std::string path = "";
		GLenum magFilter = GL_LINEAR;
		GLenum minFilter = GL_LINEAR;
		GLenum wrapS = GL_REPEAT;
		GLenum wrapT = GL_REPEAT;
		GLenum wrapR = GL_REPEAT;
		bool mipMap = true;
	}textureSettings;

	std::unordered_map<std::string, GLenum> cubemapNameToType = {
		{"px", GL_TEXTURE_CUBE_MAP_POSITIVE_X},
		{"nx", GL_TEXTURE_CUBE_MAP_NEGATIVE_X},
		{"py", GL_TEXTURE_CUBE_MAP_POSITIVE_Y},
		{"ny", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y},
		{"pz", GL_TEXTURE_CUBE_MAP_POSITIVE_Z},
		{"nz", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z}
	};

	Texture();
	Texture(GLuint _texUnit);
	Texture(const TextureSettings& _textureSettings);
	void bind();
	void initTexture();
	void setFormat();
	void resize(const Eigen::Vector2i& res);
	GLuint ID();
	void updateParameters();
	~Texture();

private:
	GLuint textureID = GL_INVALID_INDEX;
};