#include "Window.hpp"
#include "SDL3/SDL_oldnames.h"

#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>

#include <spdlog/spdlog.h>

using namespace Engine::Rendering;


void *Window::_create_window(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa, Uint8 windowMode)
{
	startLastCounter();
	if (aa > 0)
	{
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, aa);
	}

	auto *window = SDL_CreateWindow(name, res.x, res.y, flags);
	if (!window)
	{
		spdlog::critical("SDL_CreateWindow Error: {}", SDL_GetError());
		SDL_Quit();
		std::exit(1);
	}

	_setMode(window, windowMode);

	this->res = res;
	spdlog::info("Successfully opened window with name: {}", name);

	return window;
}

void Window::_destroyWindow(void* window)
{
	if (window != nullptr) {
		SDL_DestroyWindow(static_cast<SDL_Window *>(window));
		window = nullptr;
	}
}

void Window::_setMode(void *wptr, Uint8 windowMode) {
	if (wptr != nullptr) {
		auto *window = static_cast<SDL_Window *>(wptr);
		SDL_SetWindowFullscreen(window, windowMode & Fullscreen);
		SDL_SetWindowResizable(window, windowMode & Resizable);
		SDL_SetWindowBordered(window, windowMode & Bordered);

		SDL_SetWindowMouseGrab(window, windowMode & MouseGrab);
		SDL_SetWindowRelativeMouseMode(window, windowMode & MouseRelative);

		if (windowMode & MouseVisible)
			SDL_ShowCursor();
		else
			SDL_HideCursor();
	}
}