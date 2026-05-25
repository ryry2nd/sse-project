#include <engine/Engine.hpp>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "Objects.hpp"


typedef uint32_t Uint32;
typedef uint64_t Uint64;
typedef uint8_t Uint8;

using namespace Engine;
using namespace Engine::Rendering;
using namespace Engine::Helper;

bool outputFPS = false;


Material mat;
Camera cam;
Model model;

glm::vec3 camPos;
glm::vec3 camRot;

Window *win;
Shader *shader;
Mesh *cube;
Image *img;
Buff *camBuff;
Buff *modelBuff;

extern "C" void setup() {
	glm::vec2 res({900.0f, 500.0f});

	camPos = glm::vec3(0.0f, 0.0f, 3.0f);
	camRot = glm::vec3(0.0f, 0.0f, 0.0f);

	cam.proj = cameraGetProj(res);
	cam.view = cameraGetView(camPos, camRot);

	// Helper::printMat4Flat(cam.proj);

	model.model = glm::mat4(1.0f);
	model.normalMatrix = glm::transpose(glm::inverse(glm::mat3(model.model)));



	CreationFunctions::initAPI("OpenGl4.6");
	win = CreationFunctions::createWindow(res, "Game", SDL_WINDOW_RESIZABLE, 8, false, 0, true);
	shader = CreationFunctions::createShader(MODULE_PATH "/shaders/floatCube");
	// CreationFunctions::createShader(MODULE_PATH "/assets/shaders/instanceCube");
	cube = CreationFunctions::createMesh(Objects::cubeVertices, Objects::vertCount, Objects::cubeIndices, Objects::indexCount, (short[]){3,2,3}, 3);
	img = CreationFunctions::createImage(MODULE_PATH "/assets/textures/FISH.png");

	camBuff = CreationFunctions::createBuff(Buff::Type::Uniform, Buff::Frequency::Dynamic, sizeof(Camera), &cam);
	modelBuff = CreationFunctions::createBuff(Buff::Type::Uniform, Buff::Frequency::Dynamic, sizeof(Model), &model);

	// mat.images[0] = img;
	mat.shader = shader;
	mat.ubo[0] = modelBuff;
	mat.ubo[1] = camBuff;
}

extern "C" void loop() {
        float deltaTime = win->getDeltaTime();
        static float timer = 0.0f;
        static float t = 0.0f;

        t += deltaTime * 1.0f;

        float r = 0.5f + 0.3f * sin(t);
        float g = 0.5f + 0.3f * sin(t + 2.0f);
        float b = 0.5f + 0.3f * sin(t + 4.0f);

        // makes background look like a 70s disco rave while under 20 pounds of lsd
        win->setBackgroundColor({r, g, b, 1.0f});

        timer += deltaTime;

        if (timer >= 1.0f)
        {
            if (outputFPS) {
                Logging::info("FPS: {:.2f}", win->getFPS());
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

		modelBuff->write(0, sizeof(Helper::Model), &model);
		camBuff->write(0, sizeof(Helper::Camera), &cam);

		CreationFunctions::draw(&mat, cube);
}

extern "C" void event(SDL_Event *event, bool *running) {
	auto dt = win->getDeltaTime();

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
		Helper::rotateCamera(dt, &camRot, {event->motion.xrel, event->motion.yrel}, 0.1f);
	}
	if (type == SDL_EVENT_WINDOW_RESIZED)
	{
		win->updateScreenRes();
	}
}