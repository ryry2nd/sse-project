#pragma once

#include <glm/glm.hpp>
#include <string>
#include <engine/Engine.hpp>

namespace ExApi {
	inline std::string MODULE_PATH;
	inline std::string MODULE_ID;

	inline Engine::Rendering::Window *win = nullptr;
	inline Engine::Rendering::FrameBuffer *fbo = nullptr;
	inline Engine::Rendering::Font *font = nullptr;

	inline constexpr glm::vec2 res(900.0f, 500.0f);
	inline constexpr float ratio = res.x / res.y;

	struct Loader {
		void *setup;
		void *loop;
		void *event;
		void *shutdown;
	};
}