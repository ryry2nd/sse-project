#include <engine/Engine.hpp>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "Objects.hpp"
#include "GenMesh.hpp"

using namespace Engine;
using namespace Engine::Rendering;
using namespace Engine::Helper;
using namespace Engine::Logging;

bool outputFPS = false;


Material mat;
Material inMat;
Material bigMat;
Material debugMat;
CameraStruct cam;
ModelStruct model;

glm::vec3 camPos;
glm::vec3 camRot;

Window *win;
Shader *cubeShader;
Shader *cubeInstanceShader;
Shader *debugVectorShader;
Mesh *cube;
Mesh *megaCube;
Image *img;
Buff *camBuff;
Buff *modelBuff;
Buff *megaBuff;
Buff *cubeInstanceBuff;

extern "C" {
	void setup() {
		glm::vec2 res({900.0f, 500.0f});

		camPos = glm::vec3(0.0f, 0.0f, 3.0f);
		camRot = glm::vec3(0.0f, 0.0f, 0.0f);

		cam.proj = cameraGetProj(res);
		cam.view = cameraGetView(camPos, camRot);

		model = glm::mat4(1.0f);

		std::vector<Vert> vert;
		std::vector<Ind> ind;

		buildVoxelGrid(vert, ind, {100, 100, 100});


		CreationFunctions::initAPI("OpenGl4.6");
		win = CreationFunctions::createWindow(res, "Game", SDL_WINDOW_RESIZABLE, 8, false, 0, true);
		cubeShader = CreationFunctions::createShader(MODULE_PATH "/shaders/floatCube");
		cubeInstanceShader = CreationFunctions::createShader(MODULE_PATH "/shaders/instanceCube");
		debugVectorShader = CreationFunctions::createShader(MODULE_PATH "/shaders/debugVector");
		cube = CreationFunctions::createMesh(Objects::cubeVertices, Objects::vertCount, Objects::cubeIndices, Objects::indexCount);
		megaCube = CreationFunctions::createMesh(vert.data(), vert.size(), ind.data(), ind.size());
		img = CreationFunctions::createImage(MODULE_PATH "/assets/textures/FISH.png");

		camBuff = CreationFunctions::createBuff(Buff::Type::Uniform, Buff::Frequency::Dynamic, sizeof(CameraStruct), &cam);
		modelBuff = CreationFunctions::createBuff(Buff::Type::Uniform, Buff::Frequency::Dynamic, sizeof(ModelStruct), &model);

		ModelStruct bigModel = glm::mat4(1.0f);
		bigModel = glm::translate(bigModel, {0, -102, 0});

		megaBuff = CreationFunctions::createBuff(Buff::Type::Uniform, Buff::Frequency::Dynamic, sizeof(ModelStruct), &bigModel);

		mat.images[0] = img;
		mat.shader = cubeShader;
		mat.ubo[0] = modelBuff;
		mat.ubo[1] = camBuff;

		bigMat.images[0] = img;
		bigMat.shader = cubeShader;
		bigMat.ubo[0] = megaBuff;
		bigMat.ubo[1] = camBuff;

		inMat.shader = cubeInstanceShader;
		inMat.ubo[0] = camBuff;

		glm::vec3 gridSize(50,50,50);
		std::vector<glm::mat4> models;
        models.reserve(gridSize.x * gridSize.y * gridSize.z);

        for (int x = 0; x < gridSize.x; x++) {
            for (int y = 0; y < gridSize.y; y++) {
                for (int z = 0; z < gridSize.z; z++) {
                    glm::mat4 mat(1.0f);
                    mat = glm::translate(mat, {(x * 3) - ((gridSize.x * 3) / 2), (y * 3) + 10, ((z * 3) - (gridSize.z * 3)) - 20});
                    models.push_back(mat);
                }
            }
        }

		cubeInstanceBuff = CreationFunctions::createBuff(Rendering::Buff::Type::Storage, Rendering::Buff::Frequency::Dynamic, sizeof(glm::mat4)*models.size(), models.data());

		inMat.instanceCount = models.size();

		inMat.ssbo[0] = cubeInstanceBuff;

		debugMat.shader = debugVectorShader;
		debugMat.ubo[0] = camBuff;
	}

	void loop() {
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
				info("FPS: {:.2f}", win->getFPS());
			}
			timer = 0.0f;
		}

		static float rotate = 0.0f;

		rotate += 1.0f * deltaTime;

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, 0.0f));
		model = glm::rotate(model, rotate, glm::vec3({0, 1, 0}));

		int numKeys;
		const bool *keystates = Rendering::Window::getKeystates(numKeys);

		// movement
		if (keystates[SDL_SCANCODE_W])
		{
			camPos += -(getForwardVector(camRot) * deltaTime * 10.0f);
		}
		if (keystates[SDL_SCANCODE_S])
		{
			camPos += (getForwardVector(camRot) * deltaTime * 10.0f);
		}

		if (keystates[SDL_SCANCODE_D])
		{
			camPos += -(getRightVector(camRot) * deltaTime * 10.0f);
		}
		if (keystates[SDL_SCANCODE_A])
		{
			camPos += (getRightVector(camRot) * deltaTime * 10.0f);
		}

		if (keystates[SDL_SCANCODE_SPACE])
		{
			camPos += (getDownVector(camRot) * deltaTime * 10.0f);
		}
		if (keystates[SDL_SCANCODE_LCTRL])
		{
			camPos += -(getDownVector(camRot) * deltaTime * 10.0f);
		}

		cam.view = cameraGetView(camPos, camRot);

		modelBuff->write(0, sizeof(ModelStruct), &model);
		camBuff->write(0, sizeof(CameraStruct), &cam);

		CreationFunctions::draw(win, cube, &mat);
		CreationFunctions::draw(win, cube, &inMat);
		CreationFunctions::draw(win, megaCube, &bigMat);
		CreationFunctions::draw(win, nullptr, &debugMat);
	}

	void event(SDL_Event *event, bool *running) {
		auto dt = win->getDeltaTime();

		auto type = event->type;
		if (type == SDL_EVENT_QUIT)
			*running = false;
		if (type == SDL_EVENT_KEY_DOWN)
		{
			auto key = event->key.key;
			if (key == SDLK_ESCAPE)
			{
				*running = false;
			}
			if (key == SDLK_F3) {
				outputFPS = !outputFPS;
			}
		}
		if (type == SDL_EVENT_MOUSE_MOTION)
		{
			rotateCamera(dt, &camRot, {event->motion.xrel, event->motion.yrel}, 0.1f);
		}
		if (type == SDL_EVENT_WINDOW_RESIZED)
		{
			win->updateScreenRes();
		}
	}
}