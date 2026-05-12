#include "glm/ext/vector_float2.hpp"
#include <engine/Rendering/Rendering.hpp>
#include <memory>
#include <unordered_map>

using namespace Rendering;

typedef uint32_t Uint32;
typedef uint64_t Uint64;
typedef uint8_t Uint8;

std::unordered_map<std::string, std::unique_ptr<Rendering::Window>> wins;

extern "C" {
	void hostSetAPI(const char* api) {
		CreationFunctions::initAPI(api);
	}

	void hostCreateWindow(const char *name, glm::vec2 res, const char *winName, Uint32 flags, Uint32 aa=0, bool fullscreen=false, int vsync = 0, bool hideMouse = true) {
		wins[name] = CreationFunctions::createWindow(res, winName, flags, aa, fullscreen, vsync, hideMouse);
	}
	void hostShutDownAll() {
		wins.clear();
	}

	void hostWindowClearBackground() {
		for (auto &win : wins) {
			win.second->clearBackground();
		}
	}

	void hostWindowSwapBuffers() {
		for (auto &win : wins) {
			win.second->swapBuffer();
		}
	}

	void hostWindowSetBackgroundColor(const char *name, glm::vec4 color) {
		wins[name]->setBackgroundColor(color);
	}

	float hostGetWindowDeltaTime(const char *name) {
		return wins[name]->deltaTime;
	}

	float hostGetWindowFPS(const char *name) {
		return wins[name]->fps;
	}

	glm::vec2 hostGetWindowRes(const char *name) {
		return wins[name]->res;
	}

	void hostUpdateWindows() {
		for (auto &win : wins) {
			win.second->update();
		}
	}
}