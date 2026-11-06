#include <engine/Engine.hpp>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <cstring>
#include <string>
#include <random>

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

CameraStruct cam;
Buff *camBuff;

glm::vec3 camPos;
glm::vec3 camRot;

#define SPEED 10.0f
#define SPRINT 100.0f


#define SIZE 128
#define AREA SIZE * SIZE * SIZE
#define MAX_FACES 3 * AREA
#define MAX_VERTS 4 * MAX_FACES
#define MAX_INDS  6 * MAX_FACES

#define CHUNK_DISTANCE_X 8
#define CHUNK_DISTANCE_Y 2
#define CHUNK_DISTANCE_Z 8

#define COMPUTE_SHADER_THREADS 256

class Chunk {
private:
	struct Sizes {
		unsigned int v;
		unsigned int i;
	};

	static Buff *worstVert;
	static Buff *worstInd;
	static Buff *sizeB;
	static Buff *seedBuff;
	static Buff *sizesOut;
	static Buff *chunkOffset;
	static Buff *heightCache;

	static Shader *generateShader;
	static Shader *shader;

	static Image *atlas;

	Object chunk;
	Buff *chunkData;

public:
	static void init(float seed = 0.0f) {
		worstVert = createBuff(Rendering::Buff::Type::Storage, Rendering::Buff::Frequency::Dynamic, sizeof(Vert) * MAX_VERTS, nullptr);
		worstInd = createBuff(Rendering::Buff::Type::Storage, Rendering::Buff::Frequency::Dynamic, sizeof(Ind) * MAX_INDS, nullptr);

		heightCache = createBuff(Rendering::Buff::Type::Storage, Rendering::Buff::Frequency::Dynamic, sizeof(float) * AREA, nullptr);

		chunkOffset = createBuff(Rendering::Buff::Type::Uniform, Rendering::Buff::Frequency::Dynamic, sizeof(glm::vec3), nullptr);

		Uint32 s = SIZE;

		sizeB = createBuff(Rendering::Buff::Type::Uniform, Rendering::Buff::Frequency::Dynamic, sizeof(Uint32), &s);

		Sizes zeros {0, 0};

		sizesOut = createBuff(Rendering::Buff::Type::Storage, Rendering::Buff::Frequency::Dynamic, sizeof(Sizes), &zeros);

		seedBuff = createBuff(Buff::Type::Uniform, Buff::Frequency::Dynamic, sizeof(float), &seed);

		generateShader = createShader((MODULE_PATH + "/shaders/chunkGenerate").c_str(), true);
		shader = createShader((MODULE_PATH + "/shaders/hill").c_str());

		Surface atlasS;
		atlasS.width = 128;
		atlasS.height = 128 * 8;

		std::string texts[] = {"dirt.png", "grass_side.png", "grass_top.png", "stone.png", "wood_side.png", "wood_top.png", "leaves.png", "bedrock.png"};

		for (int i = 0; i < 8; i++) {
			Surface surf;
			Surface::loadFile((MODULE_PATH + "/assets/textures/" + texts[i]).c_str(), &surf);

			atlasS.pixels.insert(atlasS.pixels.end(), surf.pixels.begin(), surf.pixels.end());
		}

		ImageSettings imset;
		imset.minFilter = ImageSettings::Filter::NearestMipmapNearest;
		imset.magFilter = ImageSettings::Filter::Nearest;
		imset.maxLevel = 3;

		atlas = createImage(&atlasS, imset);
		atlasS.clear();
	}

	static void deinit() {
		removeShader(generateShader);
		removeShader(shader);
		removeImage(atlas);
		removeBuff(worstVert);
		removeBuff(worstInd);
		removeBuff(sizeB);
		removeBuff(seedBuff);
		removeBuff(sizesOut);
		removeBuff(chunkOffset);
		removeBuff(heightCache);
	}

	Chunk(glm::vec3 offset = glm::vec3(0.0f)) {
		chunkOffset->write(0, sizeof(glm::vec3), &offset);

		std::vector<float> zs(AREA, -1);
		heightCache->write(0, AREA * sizeof(float), zs.data());

		Sizes zeros {0, 0};
		sizesOut->write(0, sizeof(Sizes), &zeros);

		chunkData = createBuff(Buff::Type::Storage, Buff::Frequency::Dynamic, sizeof(Uint16) * AREA, nullptr);

		ComputeParams pms;
		pms.shader = generateShader;
		pms.ssbo[0] = worstVert;
		pms.ssbo[1] = worstInd;
		pms.ssbo[2] = sizesOut;
		pms.ssbo[3] = chunkData;
		pms.ssbo[4] = heightCache;
		pms.ubo[0] = seedBuff;
		pms.ubo[1] = sizeB;
		pms.ubo[2] = chunkOffset;
		pms.groups.x = (AREA + COMPUTE_SHADER_THREADS) / COMPUTE_SHADER_THREADS;
		pms.groups.y = 1;
		pms.groups.z = 1;

		computeCall(&pms);

		Sizes sizesize;
		sizesOut->read(0, sizeof(Sizes), &sizesize);

		Mesh *chunkMesh = createMesh(nullptr, sizesize.v, nullptr, sizesize.i);

		// Copy generated vertices
		chunkMesh->vertBuff()->copy(
			worstVert,
			0,
			0,
			sizeof(Vert) * sizesize.v
		);

		// Copy generated indices
		chunkMesh->indBuff()->copy(
			worstInd,
			0,
			0,
			sizeof(Ind) * sizesize.i
		);

		glm::mat4 chunkmd = glm::translate(glm::mat4(1.0f), offset);

		Buff *chunkbuff = createBuff(Buff::Type::Uniform, Buff::Frequency::Dynamic, sizeof(glm::mat4), &chunkmd);

		chunk.mesh[0] = chunkMesh;
		chunk.mat[0].images[0] = atlas;
		chunk.mat[0].shader = shader;
		chunk.mat[0].ubo[0] = chunkbuff;
		chunk.mat[0].ubo[1] = camBuff;
		chunk.mat[0].settings |= Material::EnableClear;
	}

	void draw(RenderTarget *t) {
		chunk.draw(t);
	}
};

Buff *Chunk::worstVert = nullptr;
Buff *Chunk::worstInd = nullptr;
Buff *Chunk::sizeB = nullptr;
Buff *Chunk::seedBuff = nullptr;
Buff *Chunk::sizesOut = nullptr;
Buff *Chunk::chunkOffset = nullptr;
Buff *Chunk::heightCache = nullptr;

Shader *Chunk::generateShader = nullptr;
Shader *Chunk::shader = nullptr;

Image *Chunk::atlas = nullptr;


std::vector<Chunk> chunks;

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

	// Image *img = createImage((MODULE_PATH + "/assets/textures/debug.png").c_str());

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


	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_real_distribution<float> dist(0.0f, 100000.0f);

	float random = dist(gen);

	Chunk::init(random);

	for (int x = 0; x < CHUNK_DISTANCE_X; x++) {
		for (int z = 0; z < CHUNK_DISTANCE_Z; z++) {
			for (int y = 0; y < CHUNK_DISTANCE_Y; y++) {
				chunks.emplace_back(glm::vec3(x * SIZE, y * SIZE, z * SIZE));
			}
		}
	}
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
	for (Chunk & c : chunks) {
		c.draw(fbo);
	}
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
	chunks.clear();
	Chunk::deinit();
}
}