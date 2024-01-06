#pragma once

#include <glad/glad.h>

class Texture
{
public:
	Texture();
	void bind();
	void setData(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* data);
	~Texture();

private:
	GLuint textureID = GL_INVALID_INDEX;
	GLenum target = GL_TEXTURE_2D;
};

Texture::Texture()
{
	glGenTextures(1, &textureID);
}

void Texture::bind()
{
	glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::setData(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* data)
{
	glBindTexture(target, textureID);
	glTexImage2D(target, level, internalFormat, width, height, border, format, type, data);
}

Texture::~Texture()
{
	glDeleteTextures(1, &textureID);
}