#pragma once

#include "Shader.hpp"
#include "Buff.hpp"
#include "Image.hpp"
#include "FrameBuffer.hpp"
#include "Mesh.hpp"
#include "Window.hpp"

namespace Engine::Rendering
{
	struct InternalParams
	{
		enum Fields : uint32_t
		{
			HasInstanceCount	= 1 << 0,
			HasSettings			= 1 << 1,
			HasUseArray     	= 1 << 2,
			HasArrayData    	= 1 << 3,
		};

		enum Settings : uint32_t {
			DisableScreen = 1 << 0,
			EnableFBO = 1 << 1,
		};

		uint32_t fieldMask = 0;

		size_t instanceCount = 0;
		uint32_t settings = 0;

		FrameBuffer* fbo = nullptr;
		Shader* shader = nullptr;

		bool useArray = false;
		Mesh::MeshTypes arrayType = Mesh::MeshTypes::Lines;
		int arrayFirst = 0;
		int arrayCount = 6;

		std::unordered_map<uint32_t, Image*> images;
		std::unordered_map<uint32_t, Buff*> ubo;
		std::unordered_map<uint32_t, Buff*> ssbo;
	};

	typedef InternalParams DrawParams;
	typedef InternalParams Material;

	namespace CreationFunctions {
		void initAPI(const char *apiName);

		void draw(Window *win, Mesh *mesh = nullptr, DrawParams *params = nullptr, size_t size = 0);
		Shader *createShader(const char *path);
		void removeShader(Shader *shader);
		Mesh *createMesh(const Vert *vertices, const size_t vert_size, const Ind *indices, const size_t ind_size, Rendering::Mesh::MeshTypes meshType = Rendering::Mesh::MeshTypes::Triangles);
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