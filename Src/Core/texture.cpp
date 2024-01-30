#include "texture.h"

Texture::Texture()
{
	glGenTextures(1, &textureID);
}

Texture::Texture(GLuint _texUnit): Texture()
{
	this->textureSettings.texUnit = _texUnit;
}

Texture::Texture(const TextureSettings& _textureSettings) : Texture()
{
	this->textureSettings = _textureSettings;
	this->initTexture();
}

void Texture::bind()
{
	glActiveTexture(GL_TEXTURE0 + this->textureSettings.texUnit);
	glBindTexture(this->textureSettings.target, textureID);
}


GLuint Texture::ID()
{
	return this->textureID;
}

/// <summary>
/// Take the path to an image for a 2d texture or a directory for a cubemap and set the texture data
/// </summary>
/// <param name="_path"></param>
void Texture::initTexture()
{	
	// check if path is a directory or a file
	if (std::filesystem::is_directory(this->textureSettings.path))
	{
		this->textureSettings.target = GL_TEXTURE_CUBE_MAP;
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		// Get all the faces of the cubemap from the directory
		std::vector<std::string> faces;
		for (const auto& entry : std::filesystem::directory_iterator(this->textureSettings.path))
			faces.push_back(entry.path().string());
		// Sort them by name, this assumes that the faces are named in the order of the cubemap enum
		std::sort(faces.begin(), faces.end());
		// if the first face is an hdr then the cubemap is hdr
		bool isHDR = faces[0].substr(faces[0].size() - 3, 3) == "hdr";
		if (isHDR)
			this->textureSettings.internalFormat = GL_FLOAT;
		this->bind();
		for (unsigned int i = 0; i < faces.size(); i++)
		{			
			if (isHDR)
			{
				this->textureSettings.type = GL_FLOAT;
				float* data = stbi_loadf(faces[i].c_str(), &this->textureSettings.width, &this->textureSettings.height, &this->textureSettings.numChannels, 0);
				this->setFormat();
				GLenum cubemapFace = this->cubemapNameToType[faces[i].substr(faces[0].size() - 6, 2)];
				if (data)
					glTexImage2D(cubemapFace, this->textureSettings.level, this->textureSettings.internalFormat, this->textureSettings.width, this->textureSettings.height, this->textureSettings.border, this->textureSettings.format, this->textureSettings.type, data);
				else
					std::cout << "Texture failed to load at path: " << faces[i] << "\n";
				stbi_image_free(data);
			}
			else
			{
				unsigned char* data = stbi_load(faces[i].c_str(), &this->textureSettings.width, &this->textureSettings.height, &this->textureSettings.numChannels, 0);
				this->setFormat();
				if (data)
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, this->textureSettings.level, this->textureSettings.internalFormat, this->textureSettings.width, this->textureSettings.height, this->textureSettings.border, this->textureSettings.format, this->textureSettings.type, data);
				else
					std::cout << "Texture failed to load at path: " << faces[i] << "\n";									
				stbi_image_free(data);
			}
		}
	}
	else
	{
		this->textureSettings.target = GL_TEXTURE_2D;
		this->bind();
		if (std::filesystem::is_regular_file(this->textureSettings.path))
		{
			unsigned char* data = stbi_load(this->textureSettings.path.c_str(), &this->textureSettings.width, &this->textureSettings.height, &this->textureSettings.numChannels, 0);
			setFormat();
			if (data)
				glTexImage2D(this->textureSettings.target, this->textureSettings.level, this->textureSettings.internalFormat, this->textureSettings.width, this->textureSettings.height, this->textureSettings.border, this->textureSettings.format, this->textureSettings.type, data);
			else
				std::cout << "Texture failed to load at path: " << this->textureSettings.path << "\n";
			stbi_image_free(data);
		}
		else if (this->textureSettings.path == "")
		{
			glTexImage2D(this->textureSettings.target, this->textureSettings.level, this->textureSettings.internalFormat, this->textureSettings.width, this->textureSettings.height, this->textureSettings.border, this->textureSettings.format, this->textureSettings.type, NULL);
		}
		else
		{
			std::cout << "Not a valid path option for texture: " << this->textureSettings.path << "\n";	
			assert(false);
		}

	}
	this->updateParameters();
}

void Texture::setFormat()
{
	switch (textureSettings.numChannels)
	{
	case 1:
		this->textureSettings.internalFormat = GL_RED;
		this->textureSettings.format = GL_RED;
		break;
	case 2:
		this->textureSettings.internalFormat = GL_RG;
		this->textureSettings.format = GL_RG;
		break;
	case 3:
		this->textureSettings.internalFormat = GL_RGB;
		this->textureSettings.format = GL_RGB;
		break;
	case 4:
		this->textureSettings.internalFormat = GL_RGBA;
		this->textureSettings.format = GL_RGBA;
		break;
	default:
		break;
	}
}

void Texture::resize(const Eigen::Vector2i& res)
{
	this->textureSettings.width = res.x();
	this->textureSettings.height = res.y();
	this->bind();
	glTexImage2D(this->textureSettings.target, this->textureSettings.level, this->textureSettings.internalFormat, this->textureSettings.width, this->textureSettings.height, this->textureSettings.border, this->textureSettings.format, this->textureSettings.type, NULL);	
	this->updateParameters();
}

GLuint64 Texture::getHandle()
{
	return 0;
}

void Texture::updateParameters()
{
	if (this->textureSettings.mipMap)
		glGenerateMipmap(this->textureSettings.target);
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