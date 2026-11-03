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

using namespace Engine;
using namespace Engine::Rendering;
using namespace Engine::Helper;
using namespace Engine::Logging;
using namespace Engine::Input;

using namespace ObjectSpace;

Object mattex;
Object mat;
Object inMat;
Object bigMat;
Object debugMat;
Object frameImageMat;
Object fboMat;
Object fpsMat;
Object monkeyMat;
Object teaMat;
Object sun;

CameraStruct cam;
Buff *camBuff;

ModelStruct model;

glm::vec3 camPos;
glm::vec3 camRot;

Window *win;
FrameBuffer *fbo;
Font *font;

std::string MODULE_PATH;
std::string MODULE_ID;

constexpr glm::vec2 res(900.0f, 500.0f);
constexpr float ratio = res.x / res.y;

extern "C" {
	void setup() {
		MODULE_ID = Engine::Module::getCurrentRunning();
		MODULE_PATH = Engine::Module::idToPath(MODULE_ID.c_str());

		if (strcmp(Engine::Module::getExe(), MODULE_ID.c_str()) != 0) {
			return;
		}

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
		buildVoxelGrid(vertsbox, indsbox, glm::vec3(1000.0f));


		initAPI("OpenGl4.6");
		win = createWindow(res, "Game", SDL_WINDOW_RESIZABLE, 8, false, 0, true);
		fbo = createFrameBuffer(res, FrameBuffer::Settings::Color | FrameBuffer::Settings::Depth);
		Shader *cubeShader = createShader((MODULE_PATH + "/shaders/floatCube").c_str());
		Shader *tex2dShader = createShader((MODULE_PATH + "/shaders/tex2d").c_str());
		Shader *cubeInstanceShader = createShader((MODULE_PATH + "/shaders/instanceCube").c_str());
		Shader *debugVectorShader = createShader((MODULE_PATH + "/shaders/debugVector").c_str());
		Shader *textShader = createShader((MODULE_PATH + "/shaders/text").c_str());
		Shader *monkeyShader = createShader((MODULE_PATH + "/shaders/monkey").c_str());
		Shader *objectShader = createShader((MODULE_PATH + "/shaders/object").c_str());
		Mesh *cube = createMesh(Objects::cubeVertices, Objects::vertCount, Objects::cubeIndices, Objects::indexCount);
		Mesh *megaCube = createMesh(vertsbox.data(), vertsbox.size(), indsbox.data(), indsbox.size());
		Image *img = createImage((MODULE_PATH + "/assets/textures/debug.png").c_str());

		font = createFont((MODULE_PATH + "/assets/fonts/Roboto-Black.ttf").c_str(), 32);
		Surface texsurf;
		font->RenderText("The quick brown fox jumps over the lazy dog.", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &texsurf);
		Image *fontimg = createImage(&texsurf, ImageSettings::tex());

		std::vector<Vert> vertsflat;
		std::vector<Ind> indsflat;

		#define TEX_SCALE 1.0f
		const float TEX_RATIO = fontimg->getWidth() / fontimg->getHeight();

		build2DFace(vertsflat, indsflat, {TEX_RATIO * TEX_SCALE, TEX_SCALE}, {0, 0, 0}, {0, 0}, {1, 1});
		Mesh *texplane = createMesh(vertsflat.data(), vertsflat.size(), indsflat.data(), indsflat.size());

		camBuff = createBuff(Buff::Type::Uniform, Buff::Frequency::Dynamic, sizeof(CameraStruct), &cam);
		Buff *modelBuff = createBuff(Buff::Type::Uniform, Buff::Frequency::Dynamic, sizeof(ModelStruct), &model);

		ModelStruct bigModel = glm::translate(glm::mat4(1.0f), {0, -1002, 0});

		Buff *megaBuff = createBuff(Buff::Type::Uniform, Buff::Frequency::Dynamic, sizeof(ModelStruct), &bigModel);

		ModelStruct texmodel = glm::translate(glm::mat4(1.0f), {3, 5, 0});

		Buff *modelTexBuff = createBuff(Buff::Type::Uniform, Buff::Frequency::Dynamic, sizeof(ModelStruct), &texmodel);

		mattex.mat[0].images[0] = fontimg;
		mattex.mat[0].shader = textShader;
		mattex.mat[0].ubo[0] = modelTexBuff;
		mattex.mat[0].ubo[1] = camBuff;
		mattex.mat[0].settings |= Material::EnableClear;
		mattex.mat[0].settings |= Material::DisableCulling;
		mattex.mesh[0] = texplane;

		mat.mat[0].images[0] = img;
		mat.mat[0].shader = cubeShader;
		mat.mat[0].ubo[0] = modelBuff;
		mat.mat[0].ubo[1] = camBuff;
		mat.mesh[0] = cube;

		bigMat.mat[0].images[0] = img;
		bigMat.mat[0].shader = cubeShader;
		bigMat.mat[0].ubo[0] = megaBuff;
		bigMat.mat[0].ubo[1] = camBuff;
		bigMat.mesh[0] = megaCube;

		inMat.mat[0].shader = cubeInstanceShader;
		inMat.mat[0].ubo[0] = camBuff;
		inMat.mesh[0] = cube;

		glm::vec3 gridSize(32);
		std::vector<ModelStruct> models;
        models.reserve(gridSize.x * gridSize.y * gridSize.z);

        for (int x = 0; x < gridSize.x; x++) {
            for (int y = 0; y < gridSize.y; y++) {
                for (int z = 0; z < gridSize.z; z++) {
					models.push_back(glm::translate(glm::mat4(1.0f), {(x * 3) - ((gridSize.x * 3) / 2), (y * 3) + 10, ((z * 3) - (gridSize.z * 3)) - 20}));
                }
            }
        }

		Buff *cubeInstanceBuff = createBuff(Rendering::Buff::Type::Storage, Rendering::Buff::Frequency::Dynamic, sizeof(ModelStruct)*models.size(), models.data());

		inMat.mat[0].instanceCount = models.size();
		inMat.mat[0].ssbo[0] = cubeInstanceBuff;

		debugMat.mat[0].shader = debugVectorShader;
		debugMat.mat[0].meshType = InternalParams::MeshTypes::Lines;
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


		std::vector<Shape> monkeyShapes;
		Mesh::getObj((MODULE_PATH + "/assets/models/Monkey.obj").c_str(), monkeyShapes);
		const Shape &mshape = monkeyShapes[0];

		Mesh *monkeyMesh = createMesh(mshape.verts.data(), mshape.verts.size(), mshape.inds.data(), mshape.inds.size());

		glm::mat4 monkeyMod = glm::translate(glm::mat4(1.0f), {3, 0, -3});

		Buff *monkeyModel = createBuff(Rendering::Buff::Type::Uniform, Rendering::Buff::Frequency::Dynamic, sizeof(glm::mat4), &monkeyMod);

		monkeyMat.mat[0].shader = monkeyShader;
		monkeyMat.mat[0].ubo[0] = monkeyModel;
		monkeyMat.mat[0].ubo[1] = camBuff;
		monkeyMat.mesh[0] = monkeyMesh;


		std::vector<Shape> teaShapes;
		Mesh::getObj((MODULE_PATH + "/assets/models/utah_teapot.obj").c_str(), teaShapes);
		const Shape &tshape = teaShapes[0];

		Mesh *teaMesh = createMesh(tshape.verts.data(), tshape.verts.size(), tshape.inds.data(), tshape.inds.size());

		glm::mat4 teaMod = glm::scale(glm::translate(glm::mat4(1.0f), {7, 0, -3}), glm::vec3(0.5f));

		Buff *teaModel = createBuff(Rendering::Buff::Type::Uniform, Rendering::Buff::Frequency::Dynamic, sizeof(glm::mat4), &teaMod);

		teaMat.mat[0].shader = monkeyShader;
		teaMat.mat[0].ubo[0] = teaModel;
		teaMat.mat[0].ubo[1] = camBuff;
		teaMat.mat[0].settings |= Material::DisableCulling;
		teaMat.mesh[0] = teaMesh;

		std::vector<Shape> sunShape;
		Mesh::getObj((MODULE_PATH + "/assets/models/gift-box-obj/gift-box.obj").c_str(), sunShape);

		struct alignas(16) Mat
		{
			glm::vec3 ambient;
			float pad0 = 0;

			glm::vec3 diffuse;
			float pad1 = 0;

			glm::vec3 specular;
			float pad2 = 0;

			glm::vec3 emissive;
			float shininess = 1.0f;

			float opacity = 1.0f;
			float ior = 1.0f;
			float pad3 = 0;
			float pad4 = 0;
		};

		glm::mat4 sunmod = glm::scale(glm::translate(glm::mat4(1.0f), {15, 0, -3}), glm::vec3(5.0f));
		Buff *sunbuff = createBuff(Rendering::Buff::Type::Uniform, Rendering::Buff::Frequency::Dynamic, sizeof(glm::mat4), &sunmod);

		for (int i = 0; i < sunShape.size(); i++) {
			const Shape &sshape = sunShape[i];

			Mat sunmatyeet;
			sunmatyeet.ambient = sshape.ambient;
			sunmatyeet.diffuse = sshape.diffuse;
			sunmatyeet.specular = sshape.specular;
			sunmatyeet.emissive = sshape.emissive;
			sunmatyeet.shininess = sshape.shininess;
			sunmatyeet.opacity = sshape.opacity;
			sunmatyeet.ior = sshape.ior;

			// sshape.print();

			Image *diffuse = createImage((MODULE_PATH + "/assets/models/gift-box-obj/" + sshape.diffuseTex).c_str());
			Image *bump = createImage((MODULE_PATH + "/assets/models/gift-box-obj/" + sshape.bumpTex).c_str());
			Image *metallic = createImage((MODULE_PATH + "/assets/models/gift-box-obj/" + sshape.metallicTex).c_str());
			Image *roughness = createImage((MODULE_PATH + "/assets/models/gift-box-obj/" + sshape.roughnessTex).c_str());

			Buff *sunMatBuff = createBuff(Rendering::Buff::Type::Uniform, Rendering::Buff::Frequency::Dynamic, sizeof(Material), &sunmatyeet);

			Material mat;
			mat.shader = objectShader;
			mat.ubo[0] = sunbuff;
			mat.ubo[1] = camBuff;
			mat.ubo[2] = sunMatBuff;
			mat.images[0] = diffuse;
			mat.images[1] = bump;
			mat.images[2] = metallic;
			mat.images[3] = roughness;
			sun.mat.push_back(mat);
			sun.mesh.push_back(createMesh(sshape.verts.data(), sshape.verts.size(), sshape.inds.data(), sshape.inds.size()));
		}
	}

	void loop() {
		if (strcmp(Engine::Module::getExe(), MODULE_ID.c_str()) != 0) {
			return;
		}
		win->update();
		win->clearBackground();
		fbo->update();
		fbo->clearBackground();
		float deltaTime = win->getDeltaTime();
		static float t = 0.0f;

		t += deltaTime * 1.0f;

		float r = 0.5f + 0.3f * sin(t);
		float g = 0.5f + 0.3f * sin(t + 2.0f);
		float b = 0.5f + 0.3f * sin(t + 4.0f);

		// makes background look like a 70s disco rave while under 20 pounds of lsd
		win->setBackgroundColor({r, g, b, 1.0f});

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
		inMat.draw(fbo);
		bigMat.draw(fbo);
		monkeyMat.draw(fbo);
		teaMat.draw(fbo);
		debugMat.draw(fbo);
		sun.draw(fbo);
		mattex.draw(fbo);
		fboMat.draw(win);
		fpsMat.draw(win);
		win->swapBuffer();
	}

	void event(Event *event) {
		if (strcmp(Engine::Module::getExe(), MODULE_ID.c_str()) != 0) {
			return;
		}
		auto dt = win->getDeltaTime();

		auto type = event->type;
		if (type == EventType::Quit)
			Engine::shutdown();
		if (type == EventType::KeyDown)
		{
			auto key = event->key;
			if (key == SDLK_ESCAPE)
				Engine::shutdown();
		}
		if (type == EventType::MouseMove)
			rotateCamera(dt, &camRot, {event->mouse.xrel, event->mouse.yrel}, 0.1f);
	}
}