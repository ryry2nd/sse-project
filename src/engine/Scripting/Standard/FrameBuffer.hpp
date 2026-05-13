#pragma once

#include <glm/ext/vector_float2.hpp>

namespace Engine::Rendering::FrameBuffer {
	enum Settings : uint32_t {
		Color = 1 << 0,
		Depth = 1 << 1,
		Stencil = 1 << 2,
	};
}

extern "C" {
	void hostCreateFrameBuffer(const char *name, glm::vec2 size, uint32_t settings);
	void hostRemoveFrameBuffer();
	void hostSetFrameBuffer(const char *name);
	glm::vec2 hostFBOGetSize();
	uint32_t hostFBOGetSettings();
	void hostFBOSetSize(glm::vec2 size);
}

namespace Engine::Rendering::FrameBuffer {
	inline void createFrameBuffer(const char *name, glm::vec2 size, uint32_t settings) {
		hostCreateFrameBuffer(name, size, settings);
	}
	inline void removeFrameBuffer() {
		hostRemoveFrameBuffer();
	}
	inline glm::vec2 getSize() {
		return hostFBOGetSize();
	}
	inline uint32_t settings() {
		return hostFBOGetSettings();
	}
	inline void setSize(glm::vec2 size) {
		hostFBOSetSize(size);
	}
}