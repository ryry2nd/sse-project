#include <engine/Engine.hpp>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <cstring>
#include <string>

#include <Objects.hpp>
#include <MathTest.hpp>
#include <FooTest.hpp>

using namespace Engine;
using namespace Engine::Rendering;
using namespace Engine::Helper;
using namespace Engine::Logging;

bool outputFPS = false;


Material mattex;
Material mat;
Material inMat;
Material bigMat;
Material debugMat;
Material frameImageMat;
CameraStruct cam;
ModelStruct model;

glm::vec3 camPos;
glm::vec3 camRot;

Window *win;
Shader *cubeShader;
Shader *cubeInstanceShader;
Shader *debugVectorShader;
Shader *textShader;
Mesh *cube;
Mesh *megaCube;
Mesh *texplane;
Image *img;
Image *fontimg;
Buff *camBuff;
Buff *modelBuff;
Buff *megaBuff;
Buff *cubeInstanceBuff;
Buff *fboBuff;
Buff *modelTexBuff;
Font *font;
FrameBuffer *fbo;

std::string MODULE_PATH;
std::string MODULE_ID;

extern "C" {
	void setup() {
		MODULE_ID = Engine::Module::getCurrentRunning();
		MODULE_PATH = Engine::Module::idToPath(MODULE_ID.c_str());

		if (strcmp(Engine::Module::getExe(), MODULE_ID.c_str()) != 0) {
			return;
		}

		int result;
		#define ad1 3
		#define ad2 2
		MathTest mathTest = MathTest();
		debug("{} + {} = {}", ad1, ad2, mathTest.add(ad1, ad2));

		foo();

		glm::vec2 res({900.0f, 500.0f});

		camPos = glm::vec3(0.0f, 0.0f, 3.0f);
		camRot = glm::vec3(0.0f, 0.0f, 0.0f);

		cam.proj = cameraGetProj(res);
		cam.view = cameraGetView(camPos, camRot);

		model = glm::mat4(1.0f);

		auto buildVoxelGrid = (void (*) (
			std::vector<Vert>& vertices,
			std::vector<Ind>& indices,
			glm::vec3 size
		)) Module::getFunction("com.ryan.std", "buildVoxelGrid");

		auto build2DFace = (void (*) (
			std::vector<Vert>& vertices,
			std::vector<Ind>& indices,
			glm::vec2 size,
			glm::vec3 position,
			glm::vec2 uvMin,
			glm::vec2 uvMax
		)) Module::getFunction("com.ryan.std", "build2DFace");


		std::vector<Vert> vertsbox;
		std::vector<Ind> indsbox;
		buildVoxelGrid(vertsbox, indsbox, glm::vec3(1000));


		initAPI("OpenGl4.6");
		win = createWindow(res, "Game", SDL_WINDOW_RESIZABLE, 8, false, 0, true);
		cubeShader = createShader((MODULE_PATH + "/shaders/floatCube").c_str());
		cubeInstanceShader = createShader((MODULE_PATH + "/shaders/instanceCube").c_str());
		debugVectorShader = createShader((MODULE_PATH + "/shaders/debugVector").c_str());
		textShader = createShader((MODULE_PATH + "/shaders/text").c_str());
		cube = createMesh(Objects::cubeVertices, Objects::vertCount, Objects::cubeIndices, Objects::indexCount);
		megaCube = createMesh(vertsbox.data(), vertsbox.size(), indsbox.data(), indsbox.size());
		img = createImage((MODULE_PATH + "/assets/textures/debug.png").c_str());

		font = new Font((MODULE_PATH + "/assets/fonts/Roboto-Black.ttf").c_str(), 64);
		auto texsurf = font->RenderText("The quick brown fox jumps over the lazy dog.", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

		fontimg = createImage(texsurf, ImageSettings::tex());
		delete font;
		// fbo = createFrameBuffer(res, FrameBuffer::Settings::Color);

		Shader *testCompute = createShader((MODULE_PATH + "/shaders/computeTest").c_str(), true);
		std::vector<int> testComputeVec(200);
		for (int i = 0; i < testComputeVec.size(); i++) {
			testComputeVec[i] = i;
		}
		Buff *testComputeBuff = createBuff(Buff::Type::Storage, Buff::Frequency::Dynamic, sizeof(int) * testComputeVec.size(), testComputeVec.data());
		ComputeParams cparams;
		cparams.shader = testCompute;
		cparams.groups = glm::vec3(4, 1, 1);
		cparams.ssbo[0] = testComputeBuff;
		computeCall(&cparams);
		testComputeBuff->read(0, testComputeVec.size(), testComputeVec.data());
		removeBuff(testComputeBuff);
		removeShader(testCompute);
		std::string out;
		for (int &n : testComputeVec) {
			out += std::to_string(n) + " ";
		}
		debug("{}", out);

		std::vector<Vert> vertsflat;
		std::vector<Ind> indsflat;

		#define TEX_SCALE 1.0f
		const float TEX_RATIO = fontimg->getWidth() / fontimg->getHeight();

		build2DFace(vertsflat, indsflat, {TEX_RATIO * TEX_SCALE, TEX_SCALE}, {0, 0, 0}, {0, 1}, {1, 0});
		texplane = createMesh(vertsflat.data(), vertsflat.size(), indsflat.data(), indsflat.size());

		camBuff = createBuff(Buff::Type::Uniform, Buff::Frequency::Dynamic, sizeof(CameraStruct), &cam);
		modelBuff = createBuff(Buff::Type::Uniform, Buff::Frequency::Dynamic, sizeof(ModelStruct), &model);

		ModelStruct bigModel = glm::translate(glm::mat4(1.0f), {0, -1002, 0});

		megaBuff = createBuff(Buff::Type::Uniform, Buff::Frequency::Dynamic, sizeof(ModelStruct), &bigModel);

		ModelStruct texmodel = glm::translate(glm::mat4(1.0f), {3, 5, 0});

		modelTexBuff = createBuff(Buff::Type::Uniform, Buff::Frequency::Dynamic, sizeof(ModelStruct), &texmodel);

		mattex.images[0] = fontimg;
		mattex.shader = textShader;
		mattex.ubo[0] = modelTexBuff;
		mattex.ubo[1] = camBuff;
		mattex.settings |= Material::EnableClear;
		mattex.settings |= Material::DisableCulling;

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

		glm::vec3 gridSize(32,32,32);
		std::vector<ModelStruct> models;
        models.reserve(gridSize.x * gridSize.y * gridSize.z);

        for (int x = 0; x < gridSize.x; x++) {
            for (int y = 0; y < gridSize.y; y++) {
                for (int z = 0; z < gridSize.z; z++) {
					models.push_back(glm::translate(glm::mat4(1.0f), {(x * 3) - ((gridSize.x * 3) / 2), (y * 3) + 10, ((z * 3) - (gridSize.z * 3)) - 20}));
                }
            }
        }

		cubeInstanceBuff = createBuff(Rendering::Buff::Type::Storage, Rendering::Buff::Frequency::Dynamic, sizeof(ModelStruct)*models.size(), models.data());

		inMat.instanceCount = models.size();
		inMat.ssbo[0] = cubeInstanceBuff;
		// inMat.settings |= InternalParams::Settings::EnableFBO;
		// inMat.fbo = fbo;

		debugMat.shader = debugVectorShader;
		debugMat.meshType = InternalParams::MeshTypes::Lines;
		debugMat.ubo[0] = camBuff;


		ModelStruct fboPosition = glm::translate(glm::mat4(1.0f), {-10, 0, 0});
		// fboBuff = createBuff(Rendering::Buff::Type::Uniform, Rendering::Buff::Frequency::Dynamic, sizeof(ModelStruct), &fboPosition);
		// frameImageMat.shader = cubeShader;
		// frameImageMat.ubo[0] = fboBuff;
		// frameImageMat.ubo[1] = camBuff;
		// frameImageMat.images[0] = fbo->getColorImage();
	}

	void loop() {
		if (strcmp(Engine::Module::getExe(), MODULE_ID.c_str()) != 0) {
			return;
		}
		win->update();
		win->clearBackground();
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

		draw(win, cube, &mat);
		draw(win, cube, &inMat);
		draw(win, megaCube, &bigMat);
		draw(win, texplane, &mattex);
		draw(win, nullptr, &debugMat);
		// draw(win, cube, &frameImageMat);
		// fbo->getColorImage()->clearTransparent();
		win->swapBuffer();
	}

	void event(SDL_Event *event) {
		if (strcmp(Engine::Module::getExe(), MODULE_ID.c_str()) != 0) {
			return;
		}
		auto dt = win->getDeltaTime();

		auto type = event->type;
		if (type == SDL_EVENT_QUIT)
			Engine::shutdown();
		if (type == SDL_EVENT_KEY_DOWN)
		{
			auto key = event->key.key;
			if (key == SDLK_ESCAPE)
				Engine::shutdown();
			if (key == SDLK_F3)
				outputFPS = !outputFPS;
		}
		if (type == SDL_EVENT_MOUSE_MOTION)
			rotateCamera(dt, &camRot, {event->motion.xrel, event->motion.yrel}, 0.1f);
	}
}