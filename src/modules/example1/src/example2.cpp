#include <engine/Engine.hpp>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <cstring>
#include <string>

#include <SDL3/SDL_events.h>

#include <Meshes.hpp>
#include <MathTest.hpp>
#include <FooTest.hpp>
#include <Object.hpp>

#include "Examples.hpp"

using namespace Engine;
using namespace Engine::Rendering;
using namespace Engine::Helper;
using namespace Engine::Logging;
using namespace Engine::Input;

using namespace ObjectSpace;

namespace Example2 {
Object mat;
Object debugMat;
Object fboMat;
Object fpsMat;

CameraStruct cam;
Buff *camBuff;

ModelStruct model;

glm::vec3 camPos;
glm::vec3 camRot;


void setup() {
	win->setMode(Window::WindowMode::WindowedMouseMode);
	camPos = glm::vec3(0.0f, 0.0f, 3.0f);
	camRot = glm::vec3(0.0f, 0.0f, 0.0f);

	cam.proj = cameraGetProj(res);
	cam.view = cameraGetView(camPos, camRot);

	model = glm::mat4(1.0f);

	auto build2DFace = (void (*) (
		std::vector<Vert>& vertices,
		std::vector<Ind>& indices,
		glm::vec2 size,
		glm::vec3 position,
		glm::vec2 uvMin,
		glm::vec2 uvMax
	)) Module::getFunction("com.ryan.std", "build2DFace");


	Shader *cubeShader = createShader((MODULE_PATH + "/shaders/floatCube").c_str());
	Shader *debugVectorShader = createShader((MODULE_PATH + "/shaders/debugVector").c_str());
	Shader *tex2dShader = createShader((MODULE_PATH + "/shaders/tex2d").c_str());
	Shader *textShader = createShader((MODULE_PATH + "/shaders/text").c_str());
	Mesh *cube = createMesh(Objects::cubeVertices, Objects::vertCount, Objects::cubeIndices, Objects::indexCount);
	Image *img = createImage((MODULE_PATH + "/assets/textures/debug.png").c_str());


	std::vector<Vert> vertsflat;
	std::vector<Ind> indsflat;

	camBuff = createBuff(Buff::Type::Uniform, Buff::Frequency::Dynamic, sizeof(CameraStruct), &cam);
	Buff *modelBuff = createBuff(Buff::Type::Uniform, Buff::Frequency::Dynamic, sizeof(ModelStruct), &model);

	mat.mat[0].images[0] = img;
	mat.mat[0].shader = cubeShader;
	mat.mat[0].ubo[0] = modelBuff;
	mat.mat[0].ubo[1] = camBuff;
	mat.mesh[0] = cube;


	debugMat.mat[0].shader = debugVectorShader;
	debugMat.mat[0].meshType = InternalParams::MeshTypes::Lines;
	debugMat.mat[0].idxCount = 6;
	debugMat.mat[0].ubo[0] = camBuff;
	debugMat.mesh[0] = nullptr;

	std::vector<Vert> vertsfbo;
	std::vector<Ind> indsfbo;

	build2DFace(vertsfbo, indsfbo, {1, 1}, {0, 0, 1}, {0, 1}, {1, 0});

	Mesh *fboMesh = createMesh(vertsfbo.data(), vertsfbo.size(), indsfbo.data(), indsfbo.size());

	vertsfbo.clear();
	indsfbo.clear();

	build2DFace(vertsfbo, indsfbo, {1, 1}, {0, 0, 1}, {0, 0}, {1, 1});

	Mesh *flatMesh = createMesh(vertsfbo.data(), vertsfbo.size(), indsfbo.data(), indsfbo.size());


	glm::mat4 fboMpv = fboMPV(res);
	Buff *fboBuff = createBuff(Rendering::Buff::Type::Uniform, Rendering::Buff::Frequency::Dynamic, sizeof(glm::mat4), &fboMpv);
	fboMat.mat[0].shader = tex2dShader;
	fboMat.mat[0].ubo[0] = fboBuff;
	fboMat.mat[0].images[0] = fbo->getColorImage();
	fboMat.mat[0].settings |= Material::EnableClear;
	fboMat.mesh[0] = fboMesh;

	glm::mat4 fpsMpv = fboMPV(res, {0, res.y - 100, 0.5}, {100, 100});
	Buff *fpsBuff = createBuff(Rendering::Buff::Type::Uniform, Rendering::Buff::Frequency::Dynamic, sizeof(glm::mat4), &fpsMpv);
	Image *fpsimg = createImage(ImageSettings::tex());
	fpsMat.mat[0].shader = tex2dShader;
	fpsMat.mat[0].ubo[0] = fpsBuff;
	fpsMat.mat[0].images[0] = fpsimg;
	fpsMat.mat[0].settings |= Material::EnableClear;
	fpsMat.mesh[0] = flatMesh;
}

void loop() {
	win->update();
	win->clearBackground();
	fbo->update();
	fbo->clearBackground();

	float deltaTime = win->getDeltaTime();

	win->setBackgroundColor({117.0f / 255.0f, 201.0f / 255.0f, 241.0f / 255.0f, 1.0f});

	static auto start = RenderTarget::getCurrTime();

	auto end = RenderTarget::getCurrTime();

	if (end - start > 100) {
		float fps = win->getFPS();
		Surface fpstexsurf;
		font->RenderText(std::format("{:.2f}", fps).c_str(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &fpstexsurf);
		Image *fpsimg = fpsMat.mat[0].images[0];
		fpsimg->clearTransparent();
		fpsimg->updateImage(&fpstexsurf);

		#define fpsScale 40
		glm::mat4 fpsMpv = fboMPV(res, {0, res.y - fpsScale, 0.5}, {(fpsimg->getWidth() / fpsimg->getHeight()) * fpsScale, fpsScale});
		fpsMat.mat[0].ubo[0]->write(0, sizeof(glm::mat4), &fpsMpv);

		start = RenderTarget::getCurrTime();
	}

	static float rotate = 0.0f;

	#define ROTATE_SPEED 1.0f

	rotate += ROTATE_SPEED * deltaTime;

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

	mat.mat[0].ubo[0]->write(0, sizeof(ModelStruct), &model);
	camBuff->write(0, sizeof(CameraStruct), &cam);

	mat.draw(fbo);
	debugMat.draw(fbo);
	fboMat.draw(win);
	fpsMat.draw(win);
	win->swapBuffer();
}

void event(Event *event) {
	auto type = event->type;
	auto dt = win->getDeltaTime();
	if (type == EventType::KeyDown) {
		auto key = event->key;
		if (key == SDLK_ESCAPE) {
			Example2::shutdown();
			Menu::setup();
			mode = Mode::Men;
			return;
		}
	}
	if (type == EventType::MouseMove)
		rotateCamera(dt, &camRot, {event->mouse.xrel, event->mouse.yrel}, 0.1f);
}

void shutdown() {
	mat.clear();
	debugMat.clear();
	fboMat.clear();
	fpsMat.clear();
}
}