#include "Window.hpp"

#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_pixels.h>

#include <spdlog/spdlog.h>

using namespace Engine::Rendering;


void Window::sdl_create_window(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa, bool fullscreen, bool hideMouse)
{
	startLastCounter();
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