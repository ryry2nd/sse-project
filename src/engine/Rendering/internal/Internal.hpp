#pragma once

#include "../Rendering.hpp"
#include <memory>

namespace Internal::Functions {
	void initAPI(const char *);

	void draw(Engine::Rendering::Window*, Engine::Rendering::Mesh*, Engine::Rendering::DrawParams*, size_t);
	std::unique_ptr<Engine::Rendering::Shader> createShader(const char *);
	std::unique_ptr<Engine::Rendering::Mesh> createMesh(const Engine::Rendering::Vert *, const size_t, const Engine::Rendering::Ind *, const size_t);
	std::unique_ptr<Engine::Rendering::Image> createImage(const char *, Engine::Rendering::ImageSettings);
	std::unique_ptr<Engine::Rendering::Image> createImage(SDL_Surface *, Engine::Rendering::ImageSettings);
	std::unique_ptr<Engine::Rendering::Window> createWindow(glm::vec2, const char *, Uint32, Uint32, bool, int, bool);
	std::unique_ptr<Engine::Rendering::Buff> createBuff(Engine::Rendering::Buff::Type, Engine::Rendering::Buff::Frequency, std::size_t, const void*);
	std::unique_ptr<Engine::Rendering::FrameBuffer> createFrameBuffer(glm::vec2, uint32_t);
}