#include <engine/Engine.hpp>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>

using namespace Engine;

#ifndef MODULE_PATH
    #define MODULE_PATH ""
#endif

bool outputFPS = false;

extern "C" void setup() {
	Rendering::Window::setAPI("OpenGl4.6");
	Rendering::Window::CreateWindow("win1", {900, 500}, "Game", SDL_WINDOW_RESIZABLE, 8, false, 0, true);
}

extern "C" void loop() {
		Rendering::Window::setWindow("win1");
        float deltaTime = Rendering::Window::getDeltaTime();
        static float timer = 0.0f;
        static int frameCount = 0;
        static float t = 0.0f;

        t += deltaTime * 1.0f;

        float r = 0.5f + 0.3f * sin(t);
        float g = 0.5f + 0.3f * sin(t + 2.0f);
        float b = 0.5f + 0.3f * sin(t + 4.0f);

        // makes background look like a 70s disco rave while under 20 pounds of lsd
        Rendering::Window::setBackgroundColor({r, g, b, 1.0f});

		frameCount++;
        timer += deltaTime;

        if (timer >= 1.0f)
        {
            if (outputFPS) {
                double fps = frameCount / timer;
                Logging::info("FPS: {:.2f}", fps);
            }

            frameCount = 0;
            timer = 0.0f;
        }
}

extern "C" void event(SDL_Event *event, bool *running) {
	Rendering::Window::setWindow("win1");
	if (event->type == SDL_EVENT_QUIT)
		*running = false;

	if (event->type == SDL_EVENT_KEY_DOWN)
	{
		SDL_Keycode key = event->key.key;
		if (key == SDLK_ESCAPE)
		{
			*running = false;
		}
		if (key == SDLK_F3) {
			outputFPS = !outputFPS;
		}
	}
	if (event->type == SDL_EVENT_WINDOW_RESIZED)
	{
		Rendering::Window::updateScreenRes();
	}
}