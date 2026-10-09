#pragma once

#include <engine/Rendering/Rendering.hpp>

namespace Engine::Rendering {
	extern "C" {
		void initAPI(const char *apiName);
		void draw(Window *win, Mesh *mesh, DrawParams *params, size_t size = 1);
		Window *createWindow(glm::vec2 res, const char *winName, Uint32 flags, Uint32 aa, bool fullscreen, int vsync, bool hideMouse);
		void removeWindow(Window *win);
		Mesh *createMesh(const Vert *vertices, const size_t vert_size, const Ind *indices, const size_t ind_size);
		void removeMesh(Mesh *mesh);
		Shader *createShader(const char *path);
		void removeShader(Shader *shdr);
		Image *createImageFromFile(const char *filePath, ImageSettings settings);
		Image *createImageFromSurf(SDL_Surface *surface, ImageSettings settings);
		void removeImage(Image *image);
		Buff *createBuff(Buff::Type type, Buff::Frequency freq, std::size_t size, const void* data);
		void removeBuff(Buff *buff);
		FrameBuffer *createFrameBuffer(glm::vec2 size, uint32_t settings);
		void removeFrameBuffer(FrameBuffer *fbo);
		void hostShutDownAll();
	}
	Image *createImage(const char *filePath, ImageSettings settings = ImageSettings{}) {
		return createImageFromFile(filePath, settings);
	}
	Image *createImage(SDL_Surface *surface, ImageSettings settings = ImageSettings{}) {
		return createImageFromSurf(surface, settings);
	}
}