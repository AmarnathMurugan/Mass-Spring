#include "framebuffer.h"


Framebuffer::Framebuffer()
{
	glGenFramebuffers(1, &id);
}

void Framebuffer::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, id);
}

void Framebuffer::attachTexture(GLuint texture, GLenum attachment)
{
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture, 0);
}

void Framebuffer::createAndAttachTexture(Texture::TextureSettings _settings, GLenum attachment, bool isResizableWithWindow)
{
	std::shared_ptr<Texture> texture = std::make_shared<Texture>(_settings);
	attachments[attachment] = texture;
	isResizeAttachmentWithWindow[attachment] = isResizableWithWindow;
	this->attachTexture(texture->ID(), attachment);
}

void Framebuffer::resizeTextures(const Eigen::Vector2i& resolution)
{
	for (auto& [attachment, tex] : attachments)
		if (isResizeAttachmentWithWindow[attachment])
			tex->resize(resolution);
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &id);
}