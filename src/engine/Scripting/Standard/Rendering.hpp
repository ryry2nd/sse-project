#pragma once

#include <glm/glm.hpp>

typedef uint32_t Uint32;
typedef uint64_t Uint64;
typedef uint8_t Uint8;

extern "C" {
	void hostSetAPI(const char* api);
	void hostCreateWindow(const char *name, glm::vec2 res, const char *winName, Uint32 flags, Uint32 aa=0, bool fullscreen=false, int vsync = 0, bool hideMouse = true);
	void hostShutDownAll();
	void hostWindowSetBackgroundColor(const char *name, glm::vec4 color);
	float hostGetWindowDeltaTime(const char *name);
	float hostGetWindowFPS(const char *name);
	glm::vec2 hostGetWindowRes(const char *name);
}

namespace Engine::Rendering {
inline void setAPI(const char* api) {
	hostSetAPI(api);
}
namespace Window {
	inline void CreateWindow(const char *name, glm::vec2 res, const char *winName, Uint32 flags, Uint32 aa=0, bool fullscreen=false, int vsync = 0, bool hideMouse = true) {
		hostCreateWindow(name,res,winName,flags,aa,fullscreen,vsync,hideMouse);
	}
	inline void setBackgroundColor(const char *name, glm::vec4 color) {
		hostWindowSetBackgroundColor(name, color);
	}
	inline float getDeltaTime(const char *name) {
		return hostGetWindowDeltaTime(name);
	}
	inline float getFPS(const char *name) {
		return hostGetWindowFPS(name);
	}
	inline glm::vec2 getWindowRes(const char *name) {
		return hostGetWindowRes(name);
	}
}
inline void ShutdownAll() {
	hostShutDownAll();
}
}