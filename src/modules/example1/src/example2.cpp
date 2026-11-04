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
#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_stdinc.h"

using namespace Engine;
using namespace Engine::Rendering;
using namespace Engine::Helper;
using namespace Engine::Logging;
using namespace Engine::Input;

using namespace ObjectSpace;

namespace Example2 {
Object debugMat;
Object fboMat;
Object fpsMat;
Object chunk;

CameraStruct cam;
Buff *camBuff;

glm::vec3 camPos;
glm::vec3 camRot;

#define SPEED 10.0f
#define SPRINT 100.0f


void setup() {
	win->setMode(Window::WindowMode::WindowedMouseMode);
	camPos = glm::vec3(0.0f, 0.0f, 3.0f);
	camRot = glm::vec3(0.0f, 0.0f, 0.0f);

	cam.proj = cameraGetProj(res);
	cam.view = cameraGetView(camPos, camRot);

	auto build2DFace = (void (*) (
		std::vector<Vert>& vertices,
		std::vector<Ind>& indices,
		glm::vec2 size,
		glm::vec3 position,
		glm::vec2 uvMin,
		glm::vec2 uvMax
	)) Module::getFunction("com.ryan.std", "build2DFace");


	Shader *debugVectorShader = createShader((MODULE_PATH + "/shaders/debugVector").c_str());
	Shader *tex2dShader = createShader((MODULE_PATH + "/shaders/tex2d").c_str());
	Shader *chunkShader = createShader((MODULE_PATH + "/shaders/hill").c_str());
	Shader *chunkComputeShader = createShader((MODULE_PATH + "/shaders/hillCompute").c_str(), true);
	// Image *img = createImage((MODULE_PATH + "/assets/textures/debug.png").c_str());

	Surface atlas;
	atlas.width = 128;
	atlas.height = 128 * 4;

	std::string texts[] = {"dirt.png", "grass_side.png", "grass_top.png", "stone.png"};

	for (int i = 0; i < 4; i++) {
		Surface surf;
		Surface::loadFile((MODULE_PATH + "/assets/textures/" + texts[i]).c_str(), &surf);

		atlas.pixels.insert(atlas.pixels.end(), surf.pixels.begin(), surf.pixels.end());
	}

	ImageSettings imset;
	imset.minFilter = ImageSettings::Filter::NearestMipmapNearest;
	imset.magFilter = ImageSettings::Filter::Nearest;
	imset.maxLevel = 5;

	Image *imgatlas = createImage(&atlas, imset);
	atlas.clear();

	std::vector<Vert> vertsflat;
	std::vector<Ind> indsflat;

	camBuff = createBuff(Buff::Type::Uniform, Buff::Frequency::Dynamic, sizeof(CameraStruct), &cam);

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

	#define SIZE 128
	#define AREA SIZE * SIZE * SIZE
	#define MAX_FACES 3 * AREA
	#define MAX_VERTS 4 * MAX_FACES
	#define MAX_INDS  6 * MAX_FACES

	Buff *worstcaseVert = createBuff(Rendering::Buff::Type::Storage, Rendering::Buff::Frequency::Dynamic, sizeof(Vert) * MAX_VERTS, nullptr);
	Buff *worstcaseInd = createBuff(Rendering::Buff::Type::Storage, Rendering::Buff::Frequency::Dynamic, sizeof(Ind) * MAX_INDS, nullptr);

	struct Sizes {
		unsigned int v;
		unsigned int i;
	};

	Sizes zeros {0, 0};

	Uint32 s = SIZE;

	Buff *sizeB = createBuff(Rendering::Buff::Type::Storage, Rendering::Buff::Frequency::Dynamic, sizeof(Uint32), &s);
	Buff *sizesOut = createBuff(Rendering::Buff::Type::Storage, Rendering::Buff::Frequency::Dynamic, sizeof(Sizes), &zeros);

	float seed = 42;
	Buff *seedBuff = createBuff(Buff::Type::Uniform, Buff::Frequency::Dynamic, sizeof(float), &seed);

	ComputeParams pms;
	pms.shader = chunkComputeShader;
	pms.ssbo[0] = worstcaseVert;
	pms.ssbo[1] = worstcaseInd;
	pms.ssbo[2] = sizesOut;
	pms.ubo[0] = seedBuff;
	pms.ubo[1] = sizeB;
	pms.groups.x = (AREA + 255) / 256;
	pms.groups.y = 1;
	pms.groups.z = 1;

	computeCall(&pms);

	Sizes sizesize;
	sizesOut->read(0, sizeof(Sizes), &sizesize);

	Mesh *chunkMesh = createMesh(nullptr, sizeof(Vert) * sizesize.v, nullptr, sizeof(Ind) * sizesize.i);

	// Copy generated vertices
	chunkMesh->vertBuff()->copy(
		worstcaseVert,
		0,
		0,
		sizeof(Vert) * sizesize.v
	);

	// Copy generated indices
	chunkMesh->indBuff()->copy(
		worstcaseInd,
		0,
		0,
		sizeof(Ind) * sizesize.i
	);

	glm::mat4 chunkmd = glm::translate(glm::mat4(1.0f), {0, -50, 0});

	Buff *chunkbuff = createBuff(Buff::Type::Uniform, Buff::Frequency::Dynamic, sizeof(glm::mat4), &chunkmd);

	chunk.mesh[0] = chunkMesh;
	chunk.mat[0].images[0] = imgatlas;
	chunk.mat[0].shader = chunkShader;
	chunk.mat[0].ubo[0] = chunkbuff;
	chunk.mat[0].ubo[1] = camBuff;


	removeShader(chunkComputeShader);
	removeBuff(worstcaseVert);
	removeBuff(worstcaseInd);
	removeBuff(sizesOut);
	removeBuff(seedBuff);
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


	int numKeys;
	const bool *keystates = Rendering::Window::getKeystates(numKeys);

	float speed;
	if (keystates[SDL_SCANCODE_LSHIFT]) {
		speed = SPRINT;
	}
	else {
		speed = SPEED;
	}

	// movement
	if (keystates[SDL_SCANCODE_W])
	{
		camPos += -(getForwardVector(camRot) * deltaTime * speed);
	}
	if (keystates[SDL_SCANCODE_S])
	{
		camPos += (getForwardVector(camRot) * deltaTime * speed);
	}

	if (keystates[SDL_SCANCODE_D])
	{
		camPos += -(getRightVector(camRot) * deltaTime * speed);
	}
	if (keystates[SDL_SCANCODE_A])
	{
		camPos += (getRightVector(camRot) * deltaTime * speed);
	}

	if (keystates[SDL_SCANCODE_SPACE])
	{
		camPos += (getDownVector(camRot) * deltaTime * speed);
	}
	if (keystates[SDL_SCANCODE_LCTRL])
	{
		camPos += -(getDownVector(camRot) * deltaTime * speed);
	}

	cam.view = cameraGetView(camPos, camRot);
	camBuff->write(0, sizeof(CameraStruct), &cam);

	debugMat.draw(fbo);
	chunk.draw(fbo);
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
	debugMat.clear();
	fboMat.clear();
	fpsMat.clear();
	chunk.clear();
}
}