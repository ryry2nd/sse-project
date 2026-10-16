#pragma once

#include "Core.hpp"
#include "RenderTarget.hpp"

namespace Engine::Rendering
{
	class Window : public RenderTarget
	{
	public:
		Window& operator=(const Window&) = delete;
		Window& operator=(Window&&) noexcept = default;
		virtual ~Window() = 0;

		virtual void setBackgroundColor(glm::vec4 color) = 0;
		virtual void swapBuffer() = 0;
		virtual void clearBackground() = 0;
	protected:
		void sdl_create_window(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa = 0, bool fullscreen = false, bool hideMouse = false);
		SDL_Window *window;
	};
}