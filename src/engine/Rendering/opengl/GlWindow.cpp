#include "GlRendering.hpp"

#include <glad/gl.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>

using namespace OpenGl;
using namespace Rendering;

#define MAJOR_VERSION 4
#define MINOR_VERSION 6

GlWindow::GlWindow(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa, bool fullscreen, int vsync, bool hideMouse)
	: Window(res, name, flags | SDL_WINDOW_OPENGL, aa, fullscreen, hideMouse)
{
	spdlog::debug("Creating window with resolution: {} by {}", res.x, res.y);
	// this does what it says
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, MAJOR_VERSION);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, MINOR_VERSION);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	spdlog::debug("Setting OpenGl version to {}.{}", MAJOR_VERSION, MINOR_VERSION);

	#ifdef DEBUG
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	#endif

	// this makes the opengl content
	glContext = SDL_GL_CreateContext(window);
	if (!glContext)
	{
		spdlog::error("SDL_GL_CreateContext Error: {}", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
		throw std::runtime_error("Had issues making the opengl context");
	}

	if (!gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress))
	{
		spdlog::error("Failed to initialize GLAD");
		SDL_GL_DestroyContext(glContext);
		SDL_DestroyWindow(window);
		SDL_Quit();
		throw std::runtime_error("Failed to initialize GLAD");
	}
	#ifdef DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	glDebugMessageCallback(
		[](GLenum source, GLenum type, GLuint id, GLenum severity,
		GLsizei length, const GLchar* message, const void* userParam)
		{
			spdlog::error("OPENGL DEBUG [{}]: {}", id, message);
		},
		nullptr
	);

	spdlog::debug("OpenGL debug output enabled.");
	#endif

	glViewport(0, 0, res.x, res.y);
	SDL_GL_SetSwapInterval(vsync);
	// this turns on all the cool opengl settings that probably should be the default
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glDepthFunc(GL_LESS); // or GL_LEQUAL
	glDepthMask(GL_TRUE); // enable writing to depth buffer
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	spdlog::debug("Created Window with glContext id: 0x{:x}", reinterpret_cast<uintptr_t>(glContext));
}

void GlWindow::setBackgroundColor(glm::vec4 color) {
	glClearColor(color.r, color.g, color.b, color.a);
}

void GlWindow::disableDepthTest() {
	glDisable(GL_DEPTH_TEST);

}
void GlWindow::enableDepthTest() {
	glEnable(GL_DEPTH_TEST);
}

void GlWindow::enableBackfaceCull() {
	glEnable(GL_CULL_FACE);
}

void GlWindow::disableBackfaceCull() {
	glDisable(GL_CULL_FACE);
}

void GlWindow::clearBackground()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GlWindow::swapBuffer()
{
	SDL_GL_SwapWindow(window);
}

void GlWindow::updateScreenRes()
{
	glViewport(0, 0, res.x, res.y);
}

GlWindow::~GlWindow()
{
	spdlog::debug("Deleting window with context id: 0x{:x}", reinterpret_cast<uintptr_t>(glContext));
	SDL_GL_DestroyContext(glContext);
}

SDL_GLContext GlWindow::getContext() {
	return glContext;
}