#include "framebuffer.h"


Framebuffer::Framebuffer()
{
	glGenFramebuffers(1, &id);
}

void Framebuffer::setDefaultFramebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Framebuffer::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	if (this->isDepthOnly)
	{
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glClear(GL_DEPTH_BUFFER_BIT);
	}
	else
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Framebuffer::attachTexture(GLuint texture, GLenum attachment)
{
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture, 0);
}

void Framebuffer::createAndAttachTexture(const Texture::TextureSettings& _settings, GLenum attachment, bool isResizableWithWindow)
{
	std::shared_ptr<Texture> texture = std::make_shared<Texture>(_settings);
	attachments[attachment] = texture;
	isResizeAttachmentWithWindow[attachment] = isResizableWithWindow;
	texture->bind();
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