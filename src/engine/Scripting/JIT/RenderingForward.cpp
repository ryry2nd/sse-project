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
}