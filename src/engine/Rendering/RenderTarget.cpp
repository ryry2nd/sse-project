#include "RenderTarget.hpp"
#include "SDL3/SDL_init.h"

#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_timer.h>
#include <spdlog/spdlog.h>

using namespace Engine::Rendering;


static Uint64 now;

Uint64 RenderTarget::getTime() {
	return now;
}

extern "C" void hostShutDownAll();

extern "C" {
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

	void Shutdown() {
		hostShutDownAll();
		SDL_Quit();
		spdlog::info("Quit out of sdl");
	}
}


void RenderTarget::update() {
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

const bool *RenderTarget::getKeystates(int &numKeys) {
	return SDL_GetKeyboardState(&numKeys);
}

void RenderTarget::startLastCounter() {
	lastCounter = SDL_GetPerformanceCounter();
}