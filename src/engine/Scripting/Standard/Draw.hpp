#pragma once

#include <cstdint>
#include <unordered_map>

namespace Engine::Rendering {
	struct Material {
		const char *shader;
		std::unordered_map<size_t, const char*> images;
		std::unordered_map<size_t, const char*> ubo;
		std::unordered_map<size_t, const char*> ssbo;
	};
	struct DrawParams {
		enum Settings : uint32_t {
			DisableScreen = 1 << 0,
			EnableFBO = 1 << 1,
		};
		std::unordered_map<size_t, const char*> ubo;
		std::unordered_map<size_t, const char*> ssbo;
		size_t instanceCount;
		uint32_t settings;
		const char *fbo = nullptr;
	};
}

extern "C" {
	void hostDraw(Engine::Rendering::Material *mat, const char *mesh = nullptr, Engine::Rendering::DrawParams *params = nullptr);
}

namespace Engine::Rendering {
	inline void Draw(Material *mat, const char *mesh = nullptr, Engine::Rendering::DrawParams *params = nullptr) {
		hostDraw(mat, mesh, params);
	}
}