#pragma once

#include "../Rendering.hpp"
#include <memory>

namespace InternalFunctions {
	void initAPI(const char *apiName);

	void draw(Engine::Rendering::Window *win, Engine::Rendering::Material *mat, Engine::Rendering::Mesh *mesh = nullptr, Engine::Rendering::DrawParams *params = nullptr);
	std::unique_ptr<Engine::Rendering::Shader> createShader(const char *path);
	std::unique_ptr<Engine::Rendering::Mesh> createMesh(const float *vertices, const size_t vert_size, const unsigned int *indices, const size_t ind_size, const short *vertLogic, const size_t vert_logic_size, Engine::Rendering::Mesh::MeshTypes meshType = Engine::Rendering::Mesh::MeshTypes::Triangles);
	std::unique_ptr<Engine::Rendering::Image> createImage(const char *filePath);
	std::unique_ptr<Engine::Rendering::Image> createImage(SDL_Surface *surface);
	std::unique_ptr<Engine::Rendering::Window> createWindow(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa = 0, bool fullscreen = false, int vsync = 0, bool hideMouse = true);
	std::unique_ptr<Engine::Rendering::Buff> createBuff(Engine::Rendering::Buff::Type type, Engine::Rendering::Buff::Frequency freq, std::size_t size, const void* data = nullptr);
	std::unique_ptr<Engine::Rendering::FrameBuffer> createFrameBuffer(glm::vec2 size, uint32_t settings);
}