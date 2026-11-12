#include "Rendering.hpp"

APIFrameBuffer::APIFrameBuffer(glm::vec2 res, uint32_t settings) RUN_IN_STATIC_THREAD
{
	SCOPE("Rendering::OpenGL::FBO::create");
	this->res = res;
	this->settings = settings;

	int width  = (int)res.x;
	int height = (int)res.y;

	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);

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

		colorImage = new APIImage(colorTex, res);
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

		depthImage = new APIImage(depthTex, res);
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

			depthImage = new APIImage(depthTex, res);
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

			stencilImage = new APIImage(stencilTex, res);
		}
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		spdlog::critical("id: 0x{:x} Framebuffer incomplete", id);
		std::exit(1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
} END_STATIC_THREAD

APIFrameBuffer::~APIFrameBuffer() RUN_IN_STATIC_THREAD
{
	SCOPE("Rendering::OpenGL::FBO::Destroy");
	if (colorImage)   delete colorImage;
	if (depthImage && depthImage != stencilImage) delete depthImage;
	if (stencilImage && stencilImage != depthImage) delete stencilImage;

	if (id) glDeleteFramebuffers(1, &id);
} END_STATIC_THREAD

void APIFrameBuffer::setSize(glm::vec2 newSize) RUN_IN_STATIC_THREAD
{
	SCOPE("Rendering::OpenGL::FBO::setSize");
	res = newSize;

	glBindFramebuffer(GL_FRAMEBUFFER, id);

	int width  = (int)res.x;
	int height = (int)res.y;

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
} END_STATIC_THREAD

void APIFrameBuffer::updateRes() RUN_IN_STATIC_THREAD {
	SCOPE("Rendering::OpenGL::FBO::updateRes");
	glViewport(0, 0, (GLsizei)res.x, (GLsizei)res.y);
} END_STATIC_THREAD

void APIFrameBuffer::setBackgroundColor(glm::vec4 color) RUN_IN_STATIC_THREAD {
	SCOPE("Rendering::OpenGL::FBO::setBackgroundColor");
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	glClearColor(color.r, color.g, color.b, color.a);
} END_STATIC_THREAD

void APIFrameBuffer::clearBackground() RUN_IN_STATIC_THREAD {
	SCOPE("Rendering::OpenGL::FBO::clearBackGround");
	glBindFramebuffer(GL_FRAMEBUFFER, id);

	GLbitfield clearMask = 0;

	if (settings & Settings::Color)
		clearMask |= GL_COLOR_BUFFER_BIT;

	if (settings & Settings::Depth)
		clearMask |= GL_DEPTH_BUFFER_BIT;

	if (settings & Settings::Stencil)
		clearMask |= GL_STENCIL_BUFFER_BIT;

	glClear(clearMask);
} END_STATIC_THREAD

extern "C" std::unique_ptr<FrameBuffer> createFrameBuffer(glm::vec2 size, uint32_t settings) {
    return std::make_unique<APIFrameBuffer>(size, settings);
}