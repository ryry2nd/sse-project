#include "Rendering.hpp"

#include <SDL3/SDL_video.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>

#define MAJOR_VERSION 4
#define MINOR_VERSION 6

APIWindow::APIWindow(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa, Uint8 mode, int vsync) RUN_IN_STATIC_THREAD
{
	SCOPE("Rendering::OpenGL::Window::create");
	window = static_cast<SDL_Window *>(_create_window(res, name, flags | SDL_WINDOW_OPENGL, aa, mode));
	// this does what it says
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, MAJOR_VERSION);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, MINOR_VERSION);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	spdlog::info("Setting OpenGl version to {}.{}", MAJOR_VERSION, MINOR_VERSION);

	#ifdef DEBUG
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	#endif

	// this makes the opengl content
	glContext = SDL_GL_CreateContext(window);
	if (!glContext)
	{
		spdlog::critical("SDL_GL_CreateContext Error: {}", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
		std::exit(1);
	}

	if (!gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress))
	{
		spdlog::critical("Failed to initialize GLAD");
		SDL_GL_DestroyContext(glContext);
		SDL_DestroyWindow(window);
		SDL_Quit();
		std::exit(1);
	}
	#ifdef DEBUG
	TracyGpuContext;
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	glDebugMessageCallback(
		[](GLenum source, GLenum type, GLuint id, GLenum severity,
		GLsizei length, const GLchar* message, const void* userParam)
		{
			spdlog::critical("OPENGL DEBUG [{}]: {}", id, message);
			std::exit(1);
		},
		nullptr
	);

	#endif

	glViewport(0, 0, res.x, res.y);
	SDL_GL_SetSwapInterval(vsync);
	// this turns on all the cool opengl settings that probably should be the default
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glDepthFunc(GL_LESS); // or GL_LEQUAL
	glDepthMask(GL_TRUE); // enable writing to depth buffer
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
} END_STATIC_THREAD

void APIWindow::setBackgroundColor(glm::vec4 color) RUN_IN_STATIC_THREAD {
	SCOPE("Rendering::OpenGL::Window::setBackgroundColor");
	glClearColor(color.r, color.g, color.b, color.a);
} END_STATIC_THREAD

void APIWindow::clearBackground() RUN_IN_STATIC_THREAD
{
	SCOPE("Rendering::OpenGL::Window::clearBackground");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
} END_STATIC_THREAD

#ifdef DEBUG
#include <tracy/TracyOpenGL.hpp>
#define GPUCOLLECT TracyGpuCollect;
#else
#define GPUCOLLECT
#endif


void APIWindow::swapBuffer() RUN_IN_STATIC_THREAD
{
	GPUCOLLECT;
	{
	SCOPE("Rendering::OpenGL::Window::swapBuffer");
	SDL_GL_SwapWindow(window);
	}
} END_STATIC_THREAD

void APIWindow::updateScreenRes() RUN_IN_STATIC_THREAD
{
	SCOPE("Rendering::OpenGL::Window::updateRes");
	int width, height;
	SDL_GetWindowSize(window, &width, &height);
	res = glm::vec2(width, height);
	glViewport(0, 0, res.x, res.y);
} END_STATIC_THREAD

void APIWindow::setWindow() RUN_IN_STATIC_THREAD {
	SCOPE("Rendering::OpenGL::Window::setWindow");
	SDL_GL_MakeCurrent(window, glContext);
} END_STATIC_THREAD

void APIWindow::setMode(Uint8 mode) RUN_IN_STATIC_THREAD {
	_setMode(window, mode);
} END_STATIC_THREAD

APIWindow::~APIWindow() RUN_IN_STATIC_THREAD
{
	SCOPE("Rendering::OpenGL::Window::delete");
	SDL_GL_DestroyContext(glContext);
	_destroyWindow(window);
} END_STATIC_THREAD

extern "C" std::unique_ptr<Window> createWindow(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa, Uint8 mode, int vsync) {
    return std::make_unique<APIWindow>(res, name, flags, aa, mode, vsync);
}