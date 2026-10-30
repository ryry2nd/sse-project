#include "RenderTarget.hpp"
#include "SDL3/SDL_init.h"

#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_timer.h>
#include <spdlog/spdlog.h>

#include "apis/internal/Scope.hpp"

using namespace Engine::Rendering;

extern "C" void hostShutDownAll();

extern "C" {
	void Init() {
		if (!SDL_Init(SDL_INIT_VIDEO))
		{
			spdlog::critical("SDL_Init Error: {}", SDL_GetError());
			std::exit(1);
		}
		spdlog::info("Initalized sdl");
	}

	void Update()
	{
		FRAMEMARK
		{
		SCOPE("Rendering::Core::pumpEvents");
		SDL_PumpEvents();
		}

	}

	void Shutdown() {
		hostShutDownAll();
		SDL_Quit();
		spdlog::info("Quit out of sdl");
	}
}


void RenderTarget::update() {
	SCOPE("Rendering::Core::updateTarget");
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

Uint64 RenderTarget::getCurrTime() {
	using namespace std::chrono;

	return static_cast<Uint64>(duration<double>(
        steady_clock::now().time_since_epoch()
    ).count() * 1000);
}