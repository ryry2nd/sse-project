#pragma once

#include "Core.hpp"

namespace Engine::Rendering
{
	class Window
	{
	public:
		Window& operator=(const Window&) = delete;
		Window& operator=(Window&&) noexcept = default;
		virtual ~Window() = 0;

		virtual void setBackgroundColor(glm::vec4 color) = 0;
		static const bool *getKeystates(int &numKeys);

		static Uint64 getTime();

		float getDeltaTime() {return deltaTime;}
		glm::vec2 getRes() {return res;}
		float getWidth() {return res.x;}
		float getHeight() {return res.y;}
		float getFPS() {return fps;}

		void update();
		virtual void clearBackground() = 0;
		virtual void swapBuffer() = 0;
	protected:
		void sdl_create_window(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa = 0, bool fullscreen = false, bool hideMouse = false);
		float fps = 0;
		glm::vec2 res;
		float deltaTime = 0;
		SDL_Window *window;

	private:
		Uint64 lastCounter;
	};
}