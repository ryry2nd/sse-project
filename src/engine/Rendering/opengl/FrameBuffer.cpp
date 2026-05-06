#include "GlRendering.hpp"

#include <glad/gl.h>

using namespace OpenGl;

GlFrameBuffer::GlFrameBuffer(glm::vec2 size, uint32_t settings)
{
	spdlog::debug("Creating frame buffer with size: {},{}", size.x, size.y);
	this->size = size;
	this->settings = settings;

	int width  = (int)size.x;
	int height = (int)size.y;

	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);

	spdlog::debug("Set FBO id to 0x{:x}", id);

	// -------------------------
	// COLOR
	// -------------------------
	if (settings & Settings::Color)
	{
		GLenum colorTex;
		glGenTextures(1, &colorTex);
		glBindTexture(GL_TEXTURE_2D, colorTex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
					 width, height, 0,
					 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER,
							   GL_COLOR_ATTACHMENT0,
							   GL_TEXTURE_2D,
							   colorTex,
							   0);

		GLenum drawBuf = GL_COLOR_ATTACHMENT0;
		glDrawBuffers(1, &drawBuf);

		colorImage = new GlImage(colorTex, size);
	}
	else
	{
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}

	// -------------------------
	// DEPTH + STENCIL
	// -------------------------
	if ((settings & Settings::Depth) && (settings & Settings::Stencil))
	{
		GLenum depthTex;
		glGenTextures(1, &depthTex);
		glBindTexture(GL_TEXTURE_2D, depthTex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8,
					 width, height, 0,
					 GL_DEPTH_STENCIL,
					 GL_UNSIGNED_INT_24_8, nullptr);

		glFramebufferTexture2D(GL_FRAMEBUFFER,
							   GL_DEPTH_STENCIL_ATTACHMENT,
							   GL_TEXTURE_2D,
							   depthTex,
							   0);

		depthImage = new GlImage(depthTex, size);
		stencilImage = depthImage;
	}
	else
	{
		if (settings & Settings::Depth)
		{
			GLenum depthTex;
			glGenTextures(1, &depthTex);
			glBindTexture(GL_TEXTURE_2D, depthTex);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24,
						 width, height, 0,
						 GL_DEPTH_COMPONENT,
						 GL_FLOAT, nullptr);

			glFramebufferTexture2D(GL_FRAMEBUFFER,
								   GL_DEPTH_ATTACHMENT,
								   GL_TEXTURE_2D,
								   depthTex,
								   0);

			depthImage = new GlImage(depthTex, size);
		}

		if (settings & Settings::Stencil)
		{
			GLenum stencilTex;
			glGenTextures(1, &stencilTex);
			glBindTexture(GL_TEXTURE_2D, stencilTex);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_STENCIL_INDEX8,
						 width, height, 0,
						 GL_STENCIL_INDEX,
						 GL_UNSIGNED_BYTE, nullptr);

			glFramebufferTexture2D(GL_FRAMEBUFFER,
								   GL_STENCIL_ATTACHMENT,
								   GL_TEXTURE_2D,
								   stencilTex,
								   0);

			stencilImage = new GlImage(stencilTex, size);
		}
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		spdlog::error("id: 0x{:x} Framebuffer incomplete", id);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GlFrameBuffer::~GlFrameBuffer()
{
	if (colorImage)   delete colorImage;
	if (depthImage && depthImage != stencilImage) delete depthImage;
	if (stencilImage && stencilImage != depthImage) delete stencilImage;

	if (id) glDeleteFramebuffers(1, &id);
}

void GlFrameBuffer::setSize(glm::vec2 newSize)
{
	size = newSize;

	glBindFramebuffer(GL_FRAMEBUFFER, id);

	int width  = (int)size.x;
	int height = (int)size.y;

	if (colorImage)
	{
		glBindTexture(GL_TEXTURE_2D, colorImage->getID());
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
					 width, height, 0,
					 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	}

	if (depthImage && depthImage != stencilImage)
	{
		glBindTexture(GL_TEXTURE_2D, depthImage->getID());
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24,
					 width, height, 0,
					 GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Rendering::Image* GlFrameBuffer::getColorImage() {
	return colorImage;
}
Rendering::Image* GlFrameBuffer::getDepthImage() {
	return depthImage;
}
Rendering::Image* GlFrameBuffer::getStencilImage() {
	return stencilImage;
}

GLuint GlFrameBuffer::getID() {
	return id;
}