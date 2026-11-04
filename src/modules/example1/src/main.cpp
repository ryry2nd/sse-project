#include <engine/Engine.hpp>
#include "Examples.hpp"

#include <SDL3/SDL_events.h>

using namespace Engine;
using namespace Engine::Rendering;
using namespace Engine::Helper;
using namespace Engine::Logging;
using namespace Engine::Input;

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

		switch (mode) {
			case Mode::Men:
				Menu::setup();
				break;
			case Mode::Ex1:
				Example1::setup();
				break;
			case Mode::Ex2:
				Example2::setup();
				break;
		}

	}

	void loop() {
		if (strcmp(Engine::Module::getExe(), MODULE_ID.c_str()) != 0) {
			return;
		}

		switch (mode) {
			case Mode::Men:
				Menu::loop();
				break;
			case Mode::Ex1:
				Example1::loop();
				break;
			case Mode::Ex2:
				Example2::loop();
				break;
		}
	}

	void event(Event *event) {
		if (strcmp(Engine::Module::getExe(), MODULE_ID.c_str()) != 0) {
			return;
		}

		auto type = event->type;
		if (type == EventType::Quit)
			Engine::shutdown();

		switch (mode) {
			case Mode::Men:
				Menu::event(event);
				break;
			case Mode::Ex1:
				Example1::event(event);
				break;
			case Mode::Ex2:
				Example2::event(event);
				break;
		}
	}
}