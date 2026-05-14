#include <engine/Engine.hpp>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "Objects.hpp"

using namespace Engine;

#ifndef MODULE_PATH
    #define MODULE_PATH ""
#endif

bool outputFPS = false;

struct Camera {
	glm::mat4 view;
	glm::mat4 proj;
};

Rendering::Material mat;

extern "C" void setup() {
	glm::vec2 res({900, 500});

	Camera cam;
	cam.proj = glm::perspective(glm::radians(90.0f), res.x / res.y, 0.1f, 100.0f);
	// cam.proj = glm::mat4(0.0f);
	cam.view = glm::translate(
		glm::mat4(1.0f),
		glm::vec3(0.0f, 0.0f, -3.0f)
	);

	glm::mat4 model = glm::mat4(1.0f);

	Rendering::Window::setAPI("OpenGl4.6");
	Rendering::Window::CreateWindow("win1", res, "Game", SDL_WINDOW_RESIZABLE, 8, false, 0, true);
	Rendering::Shader::createShader("shader1", MODULE_PATH "/assets/shaders/floatCube.slang");
	// Rendering::Shader::createShader("shader2", MODULE_PATH "/assets/shaders/instanceCube.slang");
	Rendering::Mesh::createMesh("cube", Objects::cubeVertices, Objects::vertCount, Objects::cubeIndices, Objects::indexCount, (short[]){3,2,3}, 3);
	Rendering::Image::createImage("im1", MODULE_PATH "/assets/textures/FISH.png");

	Rendering::Buff::createBuffer("cam1", Engine::Rendering::Buff::Type::Uniform, Engine::Rendering::Buff::Frequency::Dynamic, sizeof(Camera), &cam);
	Rendering::Buff::createBuffer("model", Engine::Rendering::Buff::Type::Uniform, Engine::Rendering::Buff::Frequency::Dynamic, sizeof(glm::mat4), &model);


	// mat.images[0] = "im1";
	mat.shader = "shader1";
	mat.ubo[0] = "model";
	mat.ubo[1] = "cam1";
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

		Rendering::Draw(&mat, "cube");
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