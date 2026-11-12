#include <ExApi.hpp>
#include <engine/Engine.hpp>

#include <SDL3/SDL_events.h>
#include <Object.hpp>
#include <unordered_map>

using namespace Engine;
using namespace Engine::Rendering;
using namespace Engine::Helper;
using namespace Engine::Logging;
using namespace Engine::Input;
using namespace ObjectSpace;
using namespace ExApi;

Object instructions;

std::unordered_map<int, ExApi::Loader> loaders;

int current = -1;

extern "C" void setLoader(int f, ExApi::Loader l) {
	if (f < 0) return;
	loaders[f] = l;
}

using LoaderF = void* (*)();
using LoaderE = void* (*)(Event *);

bool getLoaderFunction(ExApi::Loader &out) {
	auto it = loaders.find(current);

	if (it == loaders.end()) {
		return false;
	}

	out = it->second;

	return true;
}
void setupExample() {
	ExApi::Loader l;
	if (!getLoaderFunction(l) && !l.setup) {
		return;
	}

	LoaderF(l.setup) ();
}
void loopExample() {
	ExApi::Loader l;
	if (!getLoaderFunction(l) && !l.loop) {
		return;
	}

	LoaderF(l.loop)();
}
void shutdownExample() {
	ExApi::Loader l;
	if (!getLoaderFunction(l) && !l.shutdown) {
		return;
	}

	LoaderF(l.shutdown)();
}
void eventExample(Event *e) {
	ExApi::Loader l;
	if (!getLoaderFunction(l) && !l.event) {
		return;
	}

	LoaderE(l.event)(e);
}

extern "C" void exitModule() {
	shutdownExample();
	current = -1;

	win->setMode(Window::WindowMode::WindowedMode);
	win->setBackgroundColor({0, 0, 0, 1.0f});
}

extern "C" {
	void setup() {
		MODULE_ID = Engine::Module::getCurrentRunning();
		MODULE_PATH = Engine::Module::idToPath(MODULE_ID.c_str());

		if (strcmp(Engine::Module::getExe(), MODULE_ID.c_str()) != 0) {
			return;
		}

		initAPI("OpenGl4.6");
		win = createWindow(res,
			"Game",
			SDL_WINDOW_RESIZABLE,
			8,
			Window::WindowMode::WindowedMode,
			0);
		fbo = createFrameBuffer(res, FrameBuffer::Settings::Color | FrameBuffer::Settings::Depth);

		font = createFont((MODULE_PATH + "/assets/fonts/Roboto-Black.ttf").c_str(), 32);


		win->setMode(Window::WindowMode::WindowedMode);
		win->setBackgroundColor({0, 0, 0, 1.0f});

		auto build2DFace = (void (*) (
			std::vector<Vert>& vertices,
			std::vector<Ind>& indices,
			glm::vec2 size,
			glm::vec3 position,
			glm::vec2 uvMin,
			glm::vec2 uvMax
		)) Module::getFunction("com.ryan.std", "build2DFace");

		Surface texsurf;
		font->RenderText("Type the example you want", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &texsurf);
		Image *fontimg = createImage(&texsurf, ImageSettings::tex());

		Shader *tex2dShader = createShader((MODULE_PATH + "/shaders/tex2d").c_str());

		std::vector<Vert> vertsfbo;
		std::vector<Ind> indsfbo;

		build2DFace(vertsfbo, indsfbo, {1, 1}, {0, 0, 1}, {0, 0}, {1, 1});

		Mesh *flatMesh = createMesh(vertsfbo.data(), vertsfbo.size(), indsfbo.data(), indsfbo.size());

		glm::mat4 fpsMpv = fboMPV(res, {0, res.y - 100, 0.5}, {res.x, 100});
		Buff *fpsBuff = createBuff(Rendering::Buff::Type::Uniform, Rendering::Buff::Frequency::Dynamic, sizeof(glm::mat4), &fpsMpv);
		instructions.mat[0].shader = tex2dShader;
		instructions.mat[0].ubo[0] = fpsBuff;
		instructions.mat[0].images[0] = fontimg;
		instructions.mat[0].settings |= Material::EnableClear;
		instructions.mesh[0] = flatMesh;
	}

	void loop() {
		if (strcmp(Engine::Module::getExe(), MODULE_ID.c_str()) != 0) {
			return;
		}

		if (current < 0) {
			win->update();
			win->clearBackground();

			instructions.draw(win);

			win->swapBuffer();
		}
		else {
			loopExample();
		}
	}

	void event(Event *event) {
		if (strcmp(Engine::Module::getExe(), MODULE_ID.c_str()) != 0) {
			return;
		}

		if (current < 0) {
			auto type = event->type;
			if (type == EventType::Quit)
				Engine::shutdown();

			auto dt = win->getDeltaTime();
			if (type == EventType::KeyDown)
			{
				auto key = event->key;
				if (key == SDLK_ESCAPE) {
					Engine::shutdown();
					return;
				}
				else if (key == SDLK_1) {
					current = 1;
					setupExample();
					return;
				}
				else if (key == SDLK_2) {
					current = 2;
					setupExample();
					return;
				}
			}
		}
		else {
			eventExample(event);
		}
	}

	void shutdown() {
		instructions.clear();
	}
}