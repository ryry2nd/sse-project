#pragma once

extern "C" {
	void hostCreateShader(const char *name, const char *path);
	void hostRemoveShader(const char *name);
}

namespace Engine::Rendering::Shader {
	inline void createShader(const char *name, const char *path) {
		hostCreateShader(name, path);
	}
	inline void removeShader(const char *name) {
		hostRemoveShader(name);
	}
}