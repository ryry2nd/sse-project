#pragma once

#include "Core.hpp"

namespace Engine::Rendering
{
	class Window
	{
	public:
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		Window(Window&&) noexcept = default;
		Window& operator=(Window&&) noexcept = default;

		virtual ~Window() = 0;

		virtual void updateScreenRes() = 0;
		virtual void setBackgroundColor(glm::vec4 color) = 0;
		virtual void setWindow() = 0;
		static const bool *getKeystates(int &numKeys);

		static Uint64 getTime() {return now;}

		float getDeltaTime() {return deltaTime;}
		glm::vec2 getRes() {return res;}
		float getFPS() {return fps;}

		// internal functions
		void update();
		static void Update();
		virtual void clearBackground() = 0;
		virtual void swapBuffer() = 0;
		static void init();
		static void shutdown();

	protected:
		static Uint64 now;
		Window(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa = 0, bool fullscreen = false, bool hideMouse = false);
		float fps = 0;
		glm::vec2 res;
		float deltaTime = 0;
		SDL_Window *window;

	private:
		Uint64 lastCounter;
	};
}