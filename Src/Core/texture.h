#pragma once

#include <glad/glad.h>
#include <string>
#include <vector>
#include <filesystem>
#include <iostream>
#include <stb/stb_image.h>


class Texture
{
public:
	struct TextureSettings
	{
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
	}textureSettings;

	Texture(TextureSettings _textureSettings);
	void bind();
	void setImageData(std::string _path);
	void updateParameters();
	~Texture();

private:
	GLuint textureID = GL_INVALID_INDEX;
	GLenum target = GL_TEXTURE_2D;
};

Texture::Texture(TextureSettings _textureSettings) : textureSettings(_textureSettings)
{
	glGenTextures(1, &textureID);
}

void Texture::bind()
{
	glBindTexture(this->target, textureID);
}

/// <summary>
/// Take the path to an image for a 2d texture or a directory for a cubemap and set the texture data
/// </summary>
/// <param name="_path"></param>
void Texture::setImageData(std::string _path)
{
	this->bind();
	this->textureSettings.path = _path;
	// check if path is a directory or a file
	if (std::filesystem::is_directory(this->textureSettings.path))
	{
		this->textureSettings.target = GL_TEXTURE_CUBE_MAP;
		std::vector<std::string> faces;
		for (const auto& entry : std::filesystem::directory_iterator(this->textureSettings.path))		
			faces.push_back(entry.path().string());
		std::sort(faces.begin(), faces.end());
		bool isHDR = faces[0].substr(faces[0].size() - 3, 3) == "hdr";
		for (int i = 0; i < faces.size(); i++)
		{
			if(isHDR)
				this->textureSettings.internalFormat = GL_RGB16F;
			else
				this->textureSettings.internalFormat = GL_RGB;
			unsigned char* data = stbi_load(faces[i].c_str(), &this->textureSettings.width, &this->textureSettings.height, &this->textureSettings.numChannels, 0);
			if (data)			
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, this->textureSettings.level, this->textureSettings.internalFormat, this->textureSettings.width, this->textureSettings.height, this->textureSettings.border, this->textureSettings.format, this->textureSettings.type, data);			
			else			
				std::cout << "Texture failed to load at path: " << faces[i] << "\n";
			stbi_image_free(data);
			i++;
		}			
	}
	else
	{
		this->textureSettings.target = GL_TEXTURE_2D;
		unsigned char* data = stbi_load(this->textureSettings.path.c_str(), &this->textureSettings.width, &this->textureSettings.height, &this->textureSettings.numChannels, 0);

		if (data)		
			glTexImage2D(this->textureSettings.target, this->textureSettings.level, this->textureSettings.internalFormat, this->textureSettings.width, this->textureSettings.height, this->textureSettings.border, this->textureSettings.format, this->textureSettings.type, data);		
		else		
			std::cout << "Texture failed to load at path: " << this->textureSettings.path << "\n";
		stbi_image_free(data);
	}
	this->updateParameters();
}

void Texture::updateParameters()
{
	glTexParameteri(this->textureSettings.target, GL_TEXTURE_WRAP_S, this->textureSettings.wrapS);
	glTexParameteri(this->textureSettings.target, GL_TEXTURE_WRAP_T, this->textureSettings.wrapT);
	glTexParameteri(this->textureSettings.target, GL_TEXTURE_WRAP_R, this->textureSettings.wrapR);
	glTexParameteri(this->textureSettings.target, GL_TEXTURE_MIN_FILTER, this->textureSettings.minFilter);
	glTexParameteri(this->textureSettings.target, GL_TEXTURE_MAG_FILTER, this->textureSettings.magFilter);
}

Texture::~Texture()
{
	glDeleteTextures(1, &this->textureID);
}