#include "Window.hpp"

#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_pixels.h>

#include <spdlog/spdlog.h>

using namespace Engine::Rendering;

extern "C" void hostShutDownAll();

static Uint64 now;

Uint64 Window::getTime() {
	return now;
}

extern "C" {
	void Shutdown() {
		hostShutDownAll();
		SDL_Quit();
		spdlog::info("Quit out of sdl");
	}

	void Init() {
		now = SDL_GetTicks();
		if (!SDL_Init(SDL_INIT_VIDEO))
		{
			spdlog::critical("SDL_Init Error: {}", SDL_GetError());
			std::exit(1);
		}
		spdlog::info("Initalized sdl");
	}

	void Update()
	{
		SDL_PumpEvents();
		now = SDL_GetTicks();

	}
}

void Window::update() {
	static Uint64 fpsStart = SDL_GetPerformanceCounter();
    static float frameCount = 0;
    static float fpsSum = 0;

    Uint64 currentCounter = SDL_GetPerformanceCounter();
    Uint64 freq = SDL_GetPerformanceFrequency();

    deltaTime = (currentCounter - lastCounter) / (float)freq;

    fpsSum += 1.0f / deltaTime;
    frameCount++;

    double elapsed = (currentCounter - fpsStart) / (double)freq;

    if (elapsed >= 0.1) {
        fps = fpsSum / frameCount;
        fpsSum = 0;
        frameCount = 0;
        fpsStart = currentCounter;
    }

    lastCounter = currentCounter;
}

const bool *Window::getKeystates(int &numKeys) {
	return SDL_GetKeyboardState(&numKeys);
}

void Window::sdl_create_window(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa, bool fullscreen, bool hideMouse)
{
	lastCounter = SDL_GetPerformanceCounter();

	if (aa > 0)
	{
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, aa);
	}

	window = SDL_CreateWindow(name, res.x, res.y, flags);
	if (!window)
	{
		spdlog::critical("SDL_CreateWindow Error: {}", SDL_GetError());
		SDL_Quit();
		std::exit(1);
	}

	if (hideMouse)
		// hides the mouse
		SDL_SetWindowRelativeMouseMode(window, true);
	if (fullscreen) {
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
		SDL_SetWindowResizable(window, true);
		SDL_SetWindowBordered(window, false);
	}
	this->res = res;
	spdlog::info("Successfully opened window with name: {}", name);
}

Window::~Window()
{
	if (window != nullptr) {
		SDL_DestroyWindow(window);
		window = nullptr;
	}
}