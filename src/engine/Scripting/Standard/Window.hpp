#pragma once

#include <glm/glm.hpp>

typedef uint32_t Uint32;
typedef uint64_t Uint64;
typedef uint8_t Uint8;

extern "C" {
	void hostSetAPI(const char* api);
	void hostCreateWindow(const char *name, glm::vec2 res, const char *winName, Uint32 flags, Uint32 aa=0, bool fullscreen=false, int vsync = 0, bool hideMouse = true);
	void hostWindowSetBackgroundColor(glm::vec4 color);
	float hostGetWindowDeltaTime();
	float hostGetWindowFPS();
	glm::vec2 hostGetWindowRes();
	void hostSetWindow(const char *name);
	void hostWindowUpdateScreenRes();
	void hostWindowDisableDepthTest();
	void hostWindowEnableDepthTest();
	void hostWindowEnableBackfaceCull();
	void hostWindowDisableBackfaceCull();
	const bool *hostWindowGetKeystates(int &numKeys);
}

namespace Engine::Rendering::Window {
	inline void setAPI(const char* api) {
		hostSetAPI(api);
	}
	inline void CreateWindow(const char *name, glm::vec2 res, const char *winName, Uint32 flags, Uint32 aa=0, bool fullscreen=false, int vsync = 0, bool hideMouse = true) {
		hostCreateWindow(name,res,winName,flags,aa,fullscreen,vsync,hideMouse);
	}
	inline void setWindow(const char *name) {
		hostSetWindow(name);
	}
	inline void setBackgroundColor(glm::vec4 color) {
		hostWindowSetBackgroundColor(color);
	}
	inline float getDeltaTime() {
		return hostGetWindowDeltaTime();
	}
	inline float getFPS() {
		return hostGetWindowFPS();
	}
	inline glm::vec2 getWindowRes() {
		return hostGetWindowRes();
	}
	inline void updateScreenRes() {
		hostWindowUpdateScreenRes();
	}
	inline void disableDepthTest() {
		hostWindowDisableDepthTest();
	}
	inline void enableDepthTest() {
		hostWindowEnableDepthTest();
	}
	inline void enableBackfaceCull() {
		hostWindowEnableBackfaceCull();
	}
	inline void disableBackfaceCull() {
		hostWindowDisableBackfaceCull();
	}

	inline const bool *getKeystates(int &numKeys) {
		return hostWindowGetKeystates(numKeys);
	}
}