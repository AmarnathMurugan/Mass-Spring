#pragma once

#include <Eigen/Core>
#include <glad/glad.h>
#include <unordered_map>
#include <unordered_set>
#include "texture.h"

class Framebuffer
{
public:
	Framebuffer();
	void bind();
	void attachTexture(GLuint texture, GLenum attachment);
	void createAndAttachTexture(Texture::TextureSettings _settings, GLenum attachment, bool isResizableWithWindow);
	void resizeTextures(const Eigen::Vector2i& resolution);
	~Framebuffer();
public:
	GLuint ID() { return id; }	
	std::unordered_map<GLenum, std::shared_ptr<Texture>> attachments;
private:
	GLuint id;
	std::unordered_map<GLenum, bool> isResizeAttachmentWithWindow;
};
