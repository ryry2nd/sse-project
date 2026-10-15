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

extern "C" {
	void draw(Window *win, Mesh *mesh, DrawParams *params, size_t size) {
		getApiFunctions()->createDrawFunc(win, mesh, params, size);
	}

	void computeCall(ComputeParams *params) {
		getApiFunctions()->createDrawCompFunc(params);
	}

	Window *createWindow(glm::vec2 res, const char *winName, Uint32 flags, Uint32 aa, bool fullscreen, int vsync, bool hideMouse) {
		auto win = getApiFunctions()->createWindowFunc(res, winName, flags, aa, fullscreen, vsync, hideMouse);
		auto ret = win.get();
		wins.push_back(std::move(win));
		return ret;
	}
	void removeWindow(Window *win) {
		for (int i = 0; i < wins.size(); i++) {
			if (wins[i].get() == win) {
				wins.erase(wins.begin() + i);
				break;
			}
		}
	}

	Mesh *createMesh(const Vert *vertices, const size_t vert_size, const Ind *indices, const size_t ind_size) {
		auto mesh = getApiFunctions()->createMeshFunc(vertices, vert_size, indices, ind_size);
		auto ret = mesh.get();
		meshes.push_back(std::move(mesh));
		return ret;
	}
	void removeMesh(Mesh *mesh) {
		for (int i = 0; i < meshes.size(); i++) {
			if (meshes[i].get() == mesh) {
				meshes.erase(meshes.begin() + i);
				break;
			}
		}
	}

	Shader *createShader(const char *path, bool isCompute) {
		auto shdr = getApiFunctions()->createShaderFunc(path, isCompute);
		auto ret = shdr.get();
		shaders.push_back(std::move(shdr));
		return ret;
	}
	void removeShader(Shader *shdr) {
		for (int i = 0; i < shaders.size(); i++) {
			if (shaders[i].get() == shdr) {
				shaders.erase(shaders.begin() + i);
				break;
			}
		}
	}

	Image *createImageFromFile(const char *filePath, ImageSettings settings) {
		auto img = getApiFunctions()->createImageFromFileFunc(filePath, settings);
		auto ret = img.get();
		images.push_back(std::move(img));
		return ret;
	}
	Image *createImageFromSurf(SDL_Surface *surface, ImageSettings settings) {
		auto img = getApiFunctions()->createImageFromSurfaceFunc(surface, settings);
		auto ret = img.get();
		images.push_back(std::move(img));
		return ret;
	}
	void removeImage(Image *image) {
		for (int i = 0; i < images.size(); i++) {
			if (images[i].get() == image) {
				images.erase(images.begin() + i);
				break;
			}
		}
	}

	Buff *createBuff(Buff::Type type, Buff::Frequency freq, std::size_t size, const void* data) {
		auto buff = getApiFunctions()->createBuffFunc(type, freq, size, data);
		auto ret = buff.get();
		buffers.push_back(std::move(buff));
		return ret;
	}
	void removeBuff(Buff *buff) {
		for (int i = 0; i < buffers.size(); i++) {
			if (buffers[i].get() == buff) {
				buffers.erase(buffers.begin() + i);
				break;
			}
		}
	}

	FrameBuffer *createFrameBuffer(glm::vec2 size, uint32_t settings) {
		auto fbo = getApiFunctions()->createFboFunc(size, settings);
		auto ret = fbo.get();
		fbos.push_back(std::move(fbo));
		return ret;
	}
	void removeFrameBuffer(FrameBuffer *fbo) {
		for (int i = 0; i < fbos.size(); i++) {
			if (fbos[i].get() == fbo) {
				fbos.erase(fbos.begin() + i);
				break;
			}
		}
	}
	void hostShutDownAll() {
		meshes.clear();
		fbos.clear();
		images.clear();
		buffers.clear();
		shaders.clear();
		wins.clear();
	}
}