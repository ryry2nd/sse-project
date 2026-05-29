#pragma once

#include "Shader.hpp"
#include "Buff.hpp"
#include "Image.hpp"
#include "FrameBuffer.hpp"
#include "Mesh.hpp"
#include "Window.hpp"

namespace Engine::Rendering
{
	struct Material {
		Shader *shader;
		std::unordered_map<size_t, Image*> images;
		std::unordered_map<size_t, Buff*> ubo;
		std::unordered_map<size_t, Buff*> ssbo;
	};
	struct DrawParams {
		enum Settings : uint32_t {
			DisableScreen = 1 << 0,
			EnableFBO = 1 << 1,
		};

		std::unordered_map<size_t, Image*> images;
		std::unordered_map<size_t, Buff*> ubo;
		std::unordered_map<size_t, Buff*> ssbo;
		size_t instanceCount;
		uint32_t settings;

		FrameBuffer *fbo = nullptr;

		Mesh::MeshTypes arrayType = Mesh::MeshTypes::Lines;
		int arrayFirst;
		int arrayCount;
	};

	namespace CreationFunctions {
		void initAPI(const char *apiName);

		void draw(Window *win, Material *mat, Mesh *mesh = nullptr, DrawParams *params = nullptr);
		Shader *createShader(const char *path);
		void removeShader(Shader *shader);
		Mesh *createMesh(const float *vertices, const size_t vert_size, const unsigned int *indices, const size_t ind_size, const short *vertLogic, const size_t vert_logic_size, Rendering::Mesh::MeshTypes meshType = Rendering::Mesh::MeshTypes::Triangles);
		void removeMesh(Mesh *mesh);
		Image *createImage(const char *filePath);
		Image *createImage(SDL_Surface *surface);
		void removeImage(Image *image);
		Window *createWindow(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa = 0, bool fullscreen = false, int vsync = 0, bool hideMouse = true);
		void removeWindow(Window *window);
		Buff *createBuff(Buff::Type type, Buff::Frequency freq, std::size_t size, const void* data = nullptr);
		void removeBuff(Buff *buff);
		FrameBuffer *createFrameBuffer(glm::vec2 size, uint32_t settings);
		void removeFrameBuffer(FrameBuffer *fbo);
	}
}