#pragma once

#include <glm/ext/vector_float2.hpp>
#include <SDL3/SDL_surface.h>

extern "C" {
	void hostCreateImage(const char *name, const char *filePath);
	void hostCreateImage2(const char *name, SDL_Surface *surface);
	void hostSetImage(const char *name);
	glm::vec2 hostGetImageSizes();
	void hostImageClearTransparent();
}

namespace Engine::Rendering::Image {
	inline void createImage(const char *name, const char *filePath) {
		hostCreateImage(name, filePath);
	}
	inline void createImage(const char *name, SDL_Surface *surface) {
		hostCreateImage2(name, surface);
	}
	inline void setImage(const char *name) {
		hostSetImage(name);
	}
	inline glm::vec2 getSizes() {
		return hostGetImageSizes();
	}
	inline void clearTransparent() {
		hostImageClearTransparent();
	}
}