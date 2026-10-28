#include "Rendering.hpp"
#include <memory>
#include <vector>
#include "apis/internal/ApiFunction.hpp"

using namespace Engine::Rendering;
using namespace NApiFunctions;

std::vector<std::unique_ptr<Window>> wins;
std::vector<std::unique_ptr<Shader>> shaders;
std::vector<std::unique_ptr<Buff>> buffers;
std::vector<std::unique_ptr<Mesh>> meshes;
std::vector<std::unique_ptr<Image>> images;
std::vector<std::unique_ptr<FrameBuffer>> fbos;
std::vector<std::unique_ptr<Font>> fonts;

#ifdef DEBUG
#include <tracy/Tracy.hpp>
#define TRACK_ADD(ptr, size, pool) TracyAllocNS(ptr, size, 12, pool)
#define TRACK_DEL(ptr, pool) TracyFreeNS(ptr, 12, pool)

// Find ptr in vec, report the free, then erase it.
#define REMOVE_FROM(vec, ptr, pool)                 \
	for (size_t i = 0; i < vec.size(); i++) {       \
		if (vec[i].get() == ptr) {                  \
			TRACK_DEL(ptr, pool);                   \
			vec.erase(vec.begin() + i);             \
			break;                                  \
		}                                           \
	}

// Report frees for everything still alive, then clear.
#define CLEAR_ALL(vec, pool)                        \
	for (auto &p : vec) { TRACK_DEL(p.get(), pool); } \
	vec.clear();
#else
#define TRACK_ADD(ptr, size, pool)
#define TRACK_DEL(ptr, size, pool)

// Find ptr in vec, report the free, then erase it.
#define REMOVE_FROM(vec, ptr, pool)                 \
	for (size_t i = 0; i < vec.size(); i++) {       \
		if (vec[i].get() == ptr) {                  \
			vec.erase(vec.begin() + i);             \
			break;                                  \
		}                                           \
	}

// Report frees for everything still alive, then clear.
#define CLEAR_ALL(vec, pool) vec.clear();
#endif

extern "C" {
	void draw(RenderTarget *target, Mesh *mesh, DrawParams *params, size_t size) {
		getApiFunctions()->createDrawFunc(target, mesh, params, size);
	}

	void computeCall(ComputeParams *params) {
		getApiFunctions()->createDrawCompFunc(params);
	}

	Window *createWindow(glm::vec2 res, const char *winName, Uint32 flags, Uint32 aa, bool fullscreen, int vsync, bool hideMouse) {
		auto win = getApiFunctions()->createWindowFunc(res, winName, flags, aa, fullscreen, vsync, hideMouse);
		auto ret = win.get();
		TRACK_ADD(ret, sizeof(Window), "Windows");
		wins.push_back(std::move(win));
		return ret;
	}
	void removeWindow(Window *win) { REMOVE_FROM(wins, win, "Windows") }

	Mesh *createMesh(const Vert *vertices, const size_t vert_size, const Ind *indices, const size_t ind_size) {
		auto mesh = getApiFunctions()->createMeshFunc(vertices, vert_size, indices, ind_size);
		auto ret = mesh.get();
		TRACK_ADD(ret, sizeof(Mesh) + (vert_size * sizeof(Vert)) + (ind_size * sizeof(Ind)), "Meshes");
		meshes.push_back(std::move(mesh));
		return ret;
	}
	void removeMesh(Mesh *mesh) { REMOVE_FROM(meshes, mesh, "Meshes") }

	Shader *createShader(const char *path, bool isCompute) {
		auto shdr = getApiFunctions()->createShaderFunc(path, isCompute);
		auto ret = shdr.get();
		TRACK_ADD(ret, sizeof(Shader), "Shaders");
		shaders.push_back(std::move(shdr));
		return ret;
	}
	void removeShader(Shader *shdr) { REMOVE_FROM(shaders, shdr, "Shaders") }

	Image *createImageFromFile(const char *filePath, ImageSettings settings) {
		auto img = getApiFunctions()->createImageFromFileFunc(filePath, settings);
		auto ret = img.get();
		TRACK_ADD(ret, sizeof(Image) + (ret->getWidth() * ret->getHeight() * 4), "Images");
		images.push_back(std::move(img));
		return ret;
	}
	Image *createImageFromSurf(SDL_Surface *surface, ImageSettings settings) {
		auto img = getApiFunctions()->createImageFromSurfaceFunc(surface, settings);
		auto ret = img.get();
		TRACK_ADD(ret, sizeof(Image) + (ret->getWidth() * ret->getHeight() * 4), "Images");
		images.push_back(std::move(img));
		return ret;
	}
	Image *createImageFromSize(glm::vec2 size, ImageSettings settings) {
		auto img = getApiFunctions()->createImageFromSizeFunc(size, settings);
		auto ret = img.get();
		TRACK_ADD(ret, sizeof(Image) + (ret->getWidth() * ret->getHeight() * 4), "Images");
		images.push_back(std::move(img));
		return ret;
	}
	void removeImage(Image *image) { REMOVE_FROM(images, image, "Images") }

	Buff *createBuff(Buff::Type type, Buff::Frequency freq, std::size_t size, const void* data) {
		auto buff = getApiFunctions()->createBuffFunc(type, freq, size, data);
		auto ret = buff.get();
		TRACK_ADD(ret, sizeof(Buff) + size, "Buffers");
		buffers.push_back(std::move(buff));
		return ret;
	}
	void removeBuff(Buff *buff) { REMOVE_FROM(buffers, buff, "Buffers") }

	FrameBuffer *createFrameBuffer(glm::vec2 size, uint32_t settings) {
		auto fbo = getApiFunctions()->createFboFunc(size, settings);
		auto ret = fbo.get();
		TRACK_ADD(ret, sizeof(FrameBuffer), "FBOs");
		fbos.push_back(std::move(fbo));
		return ret;
	}
	void removeFrameBuffer(FrameBuffer *fbo) { REMOVE_FROM(fbos, fbo, "FBOs") }

	Font *createFont(const char* path, size_t size) {
		auto font = std::make_unique<Font>(path, size);
		auto ret = font.get();
		TRACK_ADD(ret, sizeof(Font), "Fonts");
		fonts.push_back(std::move(font));
		return ret;
	}
	void removeFont(Font *font) { REMOVE_FROM(fonts, font, "Fonts") }

	void hostShutDownAll() {
		CLEAR_ALL(fonts, "Fonts")
		CLEAR_ALL(meshes, "Meshes")
		CLEAR_ALL(fbos, "FBOs")
		CLEAR_ALL(images, "Images")
		CLEAR_ALL(buffers, "Buffers")
		CLEAR_ALL(shaders, "Shaders")
		CLEAR_ALL(wins, "Windows")
	}
}