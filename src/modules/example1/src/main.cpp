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


Rendering::Material mat;
Helper::Camera cam;
Helper::Model model;

glm::vec3 camPos;
glm::vec3 camRot;

extern "C" void setup() {
	glm::vec2 res({900.0f, 500.0f});

	camPos = glm::vec3(0.0f, 0.0f, 3.0f);
	camRot = glm::vec3(0.0f, 0.0f, 0.0f);

	cam.proj = Helper::cameraGetProj(res);
	cam.view = Helper::cameraGetView(camPos, camRot);

	// Helper::printMat4Flat(cam.proj);

	model.model = glm::mat4(1.0f);
	model.normalMatrix = glm::transpose(glm::inverse(glm::mat3(model.model)));



	Rendering::Window::setAPI("OpenGl4.6");
	Rendering::Window::CreateWindow("win1", res, "Game", SDL_WINDOW_RESIZABLE, 8, false, 0, true);
	Rendering::Shader::createShader("shader1", MODULE_PATH "/assets/shaders/floatCube.slang");
	// Rendering::Shader::createShader("shader2", MODULE_PATH "/assets/shaders/instanceCube.slang");
	Rendering::Mesh::createMesh("cube", Objects::cubeVertices, Objects::vertCount, Objects::cubeIndices, Objects::indexCount, (short[]){3,2,3}, 3);
	// Rendering::Image::createImage("im1", MODULE_PATH "/assets/textures/FISH.png");

	Rendering::Buff::createBuffer("cam1", Rendering::Buff::Type::Uniform, Rendering::Buff::Frequency::Dynamic, sizeof(Helper::Camera), &cam);
	Rendering::Buff::createBuffer("model", Rendering::Buff::Type::Uniform, Rendering::Buff::Frequency::Dynamic, sizeof(Helper::Model), &model);


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

		static float rotate = 0.0f;

		rotate += 1.0f * deltaTime;

		model.model = glm::mat4(1.0f);
		model.model = glm::rotate(model.model, rotate, glm::vec3({0, 0, 1}));
		model.normalMatrix = glm::transpose(glm::inverse(glm::mat3(model.model)));

		int numKeys;
        const bool *keystates = Rendering::Window::getKeystates(numKeys);

        // movement
        if (keystates[SDL_SCANCODE_W])
        {
            camPos += -((Helper::getForwardVector(camRot) * deltaTime) * 10.0f);
        }
        if (keystates[SDL_SCANCODE_S])
        {
            camPos += ((Helper::getForwardVector(camRot) * deltaTime) * 10.0f);
        }

        if (keystates[SDL_SCANCODE_D])
        {
            camPos += -((Helper::getRightVector(camRot) * deltaTime) * 10.0f);
        }
        if (keystates[SDL_SCANCODE_A])
        {
            camPos += ((Helper::getRightVector(camRot) * deltaTime) * 10.0f);
        }

        if (keystates[SDL_SCANCODE_SPACE])
        {
            camPos += ((Helper::getDownVector(camRot) * deltaTime) * 10.0f);
        }
        if (keystates[SDL_SCANCODE_LCTRL])
        {
            camPos += -((Helper::getDownVector(camRot) * deltaTime) * 10.0f);
        }

		cam.view = Helper::cameraGetView(camPos, camRot);

		Rendering::Buff::setBuffer("model");
		Rendering::Buff::write(0, sizeof(Helper::Model), &model);
		Rendering::Buff::setBuffer("cam1");
		Rendering::Buff::write(0, sizeof(Helper::Camera), &cam);

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
	if (event->type == SDL_EVENT_MOUSE_MOTION)
	{
		Helper::rotateCamera(&camRot, {event->motion.xrel, event->motion.yrel}, 0.1f);
	}
	if (type == SDL_EVENT_WINDOW_RESIZED)
	{
		Rendering::Window::setWindow("win1");
		Rendering::Window::updateScreenRes();
	}
}