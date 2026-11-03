#pragma once

#include "Core.hpp"
#include "RenderTarget.hpp"

namespace Engine::Rendering
{
	class Window : public RenderTarget
	{
	public:
		enum WindowMode : Uint8
		{
			Fullscreen 			= 1 << 0,
			Resizable			= 1 << 1,
			Bordered			= 1 << 2,
			MouseGrab 			= 1 << 3,
			MouseRelative   	= 1 << 4,
			MouseVisible    	= 1 << 5,

			FullscreenMode  	= Fullscreen | MouseGrab | MouseRelative,
			WindowedMode    	= Resizable | Bordered | MouseVisible,
			BorderlessMode  	= MouseGrab | MouseRelative,
			WindowedMouseMode 	= Resizable | Bordered | MouseRelative,
		};

		Window& operator=(const Window&) = delete;
		Window& operator=(Window&&) noexcept = default;
		virtual ~Window() = default;

		virtual void setBackgroundColor(glm::vec4 color) = 0;
		virtual void swapBuffer() = 0;
		virtual void clearBackground() = 0;
		virtual void setMode(Uint8) = 0;
	protected:
		void *_create_window(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa, Uint8 windowMode);
		void _destroyWindow(void *);
		void _setMode(void *wptr, Uint8);
	};
}