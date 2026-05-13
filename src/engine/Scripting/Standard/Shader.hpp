#pragma once

extern "C" {
	void hostCreateShader(const char *name, const char *path);
}

namespace Engine::Rendering::Shader {
	inline void createShader(const char *name, const char *path) {
		hostCreateShader(name, path);
	}
}