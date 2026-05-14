#include <engine/Engine.hpp>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <string>
#include "Objects.hpp"

using namespace Engine;

#ifndef MODULE_PATH
    #define MODULE_PATH ""
#endif

bool outputFPS = false;

extern "C" void setup() {
	Rendering::Window::setAPI("OpenGl4.6");
	Rendering::Window::CreateWindow("win1", {900, 500}, "Game", SDL_WINDOW_RESIZABLE, 8, false, 0, true);
	Rendering::Shader::createShader("shader1", MODULE_PATH "/assets/shaders/floatCube.slang");
	Rendering::Shader::createShader("shader2", MODULE_PATH "/assets/shaders/instanceCube.slang");
	Rendering::Mesh::createMesh("cube", Objects::cubeVertices, Objects::vertCount, Objects::cubeIndices, Objects::indexCount, (short[]){3,2,3}, 3);
	Rendering::Image::createImage("im1", MODULE_PATH "/assets/textures/FISH.png");
}

extern "C" void loop() {
		Rendering::Window::setWindow("win1");
        float deltaTime = Rendering::Window::getDeltaTime();
        static float timer = 0.0f;
        static float t = 0.0f;

        t += deltaTime * 1.0f;

        float r = 0.5f + 0.3f * sin(t);
        float g = 0.5f + 0.3f * sin(t + 2.0f);
        float b = 0.5f + 0.3f * sin(t + 4.0f);

        // makes background look like a 70s disco rave while under 20 pounds of lsd
        Rendering::Window::setBackgroundColor({r, g, b, 1.0f});

        timer += deltaTime;

        if (timer >= 1.0f)
        {
            if (outputFPS) {
                Logging::info("FPS: {:.2f}", Rendering::Window::getFPS());
            }
            timer = 0.0f;
        }
}

extern "C" void event(SDL_Event *event, bool *running) {
	auto type = event->type;
	if (type == SDL_EVENT_QUIT)
		*running = false;
	if (type == SDL_EVENT_KEY_DOWN)
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
	if (type == SDL_EVENT_WINDOW_RESIZED)
	{
		Rendering::Window::setWindow("win1");
		Rendering::Window::updateScreenRes();
	}
}