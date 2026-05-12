#include <glm/ext/vector_float2.hpp>
#include <engine/Rendering/Rendering.hpp>
#include <memory>
#include <unordered_map>

using namespace Rendering;

typedef uint32_t Uint32;
typedef uint64_t Uint64;
typedef uint8_t Uint8;

std::unordered_map<std::string, std::unique_ptr<Rendering::Window>> wins;

Rendering::Window *cacheWin;

extern "C" {
	void hostSetAPI(const char* api) {
		CreationFunctions::initAPI(api);
	}

	void hostCreateWindow(const char *name, glm::vec2 res, const char *winName, Uint32 flags, Uint32 aa=0, bool fullscreen=false, int vsync = 0, bool hideMouse = true) {
		auto win = CreationFunctions::createWindow(res, winName, flags, aa, fullscreen, vsync, hideMouse);
		cacheWin = win.get();
		wins[name] = std::move(win);
	}
	void hostSetWindow(const char *name) {
		cacheWin = wins[name].get();
	}
	void hostShutDownAll() {
		cacheWin = nullptr;
		wins.clear();
	}

	void hostWindowClearBackground() {
		for (auto &win : wins) {
			win.second->clearBackground();
		}
	}

	void hostWindowUpdateScreenRes() {
		cacheWin->updateScreenRes();
	}

	void hostWindowSwapBuffers() {
		for (auto &win : wins) {
			win.second->swapBuffer();
		}
	}

	void hostWindowSetBackgroundColor(glm::vec4 color) {
		cacheWin->setBackgroundColor(color);
	}

	float hostGetWindowDeltaTime() {
		return cacheWin->deltaTime;
	}

	float hostGetWindowFPS() {
		return cacheWin->fps;
	}

	glm::vec2 hostGetWindowRes() {
		return cacheWin->res;
	}

	void hostWindowDisableDepthTest() {
		cacheWin->disableDepthTest();
	}
	void hostWindowEnableDepthTest() {
		cacheWin->enableDepthTest();
	}
	void hostWindowEnableBackfaceCull() {
		cacheWin->enableBackfaceCull();
	}
	void hostWindowDisableBackfaceCull() {
		cacheWin->disableBackfaceCull();
	}

	void hostUpdateWindows() {
		for (auto &win : wins) {
			win.second->update();
		}
	}
	const bool *hostWindowGetKeystates(int &numKeys) {
		return Window::getKeystates(numKeys);
	}
}