#include "Rendering.hpp"
#include "internal/Internal.hpp"

using namespace Engine::Rendering;

typedef uint32_t Uint32;
typedef uint64_t Uint64;
typedef uint8_t Uint8;

std::vector<std::unique_ptr<Window>> wins;
std::vector<std::unique_ptr<Shader>> shaders;
std::vector<std::unique_ptr<Buff>> buffers;
std::vector<std::unique_ptr<Mesh>> meshes;
std::vector<std::unique_ptr<Image>> images;
std::vector<std::unique_ptr<FrameBuffer>> fbos;



void CreationFunctions::draw(Window *win, Material *mat, Mesh *mesh, DrawParams *params) {
	InternalFunctions::draw(win, mat, mesh, params);
}


void CreationFunctions::initAPI(const char* api) {
	InternalFunctions::initAPI(api);
}

Window *CreationFunctions::createWindow(glm::vec2 res, const char *winName, Uint32 flags, Uint32 aa, bool fullscreen, int vsync, bool hideMouse) {
	auto win = InternalFunctions::createWindow(res, winName, flags, aa, fullscreen, vsync, hideMouse);
	auto ret = win.get();
	wins.push_back(std::move(win));
	return ret;
}
void CreationFunctions::removeWindow(Window *win) {
	for (int i = 0; i < wins.size(); i++) {
		if (wins[i].get() == win) {
			wins.erase(wins.begin() + i);
			break;
		}
	}
}

Mesh *CreationFunctions::createMesh(const float *vertices, const size_t vert_size, const unsigned int *indices, const size_t ind_size, const short *vertLogic, const size_t vert_logic_size, Mesh::MeshTypes meshType) {
	auto mesh = InternalFunctions::createMesh(vertices, vert_size, indices, ind_size, vertLogic, vert_logic_size, meshType);
	auto ret = mesh.get();
	meshes.push_back(std::move(mesh));
	return ret;
}
void CreationFunctions::removeMesh(Mesh *mesh) {
	for (int i = 0; i < meshes.size(); i++) {
		if (meshes[i].get() == mesh) {
			meshes.erase(meshes.begin() + i);
			break;
		}
	}
}

Shader *CreationFunctions::createShader(const char *path) {
	auto shdr = InternalFunctions::createShader(path);
	auto ret = shdr.get();
	shaders.push_back(std::move(shdr));
	return ret;
}
void CreationFunctions::removeShader(Shader *shdr) {
	for (int i = 0; i < shaders.size(); i++) {
		if (shaders[i].get() == shdr) {
			shaders.erase(shaders.begin() + i);
			break;
		}
	}
}

Image *CreationFunctions::createImage(const char *filePath) {
	auto img = InternalFunctions::createImage(filePath);
	auto ret = img.get();
	images.push_back(std::move(img));
	return ret;
}
Image *CreationFunctions::createImage(SDL_Surface *surface) {
	auto img = InternalFunctions::createImage(surface);
	auto ret = img.get();
	images.push_back(std::move(img));
	return ret;
}
void CreationFunctions::removeImage(Image *image) {
	for (int i = 0; i < images.size(); i++) {
		if (images[i].get() == image) {
			images.erase(images.begin() + i);
			break;
		}
	}
}

Buff *CreationFunctions::createBuff(Buff::Type type, Buff::Frequency freq, std::size_t size, const void* data) {
	auto buff = InternalFunctions::createBuff(type, freq, size, data);
	auto ret = buff.get();
	buffers.push_back(std::move(buff));
	return ret;
}
void CreationFunctions::removeBuff(Buff *buff) {
	for (int i = 0; i < buffers.size(); i++) {
		if (buffers[i].get() == buff) {
			buffers.erase(buffers.begin() + i);
			break;
		}
	}
}

FrameBuffer *CreationFunctions::createFrameBuffer(glm::vec2 size, uint32_t settings) {
	auto fbo = InternalFunctions::createFrameBuffer(size, settings);
	auto ret = fbo.get();
	fbos.push_back(std::move(fbo));
	return ret;
}
void CreationFunctions::removeFrameBuffer(FrameBuffer *fbo) {
	for (int i = 0; i < fbos.size(); i++) {
		if (fbos[i].get() == fbo) {
			fbos.erase(fbos.begin() + i);
			break;
		}
	}
}

extern "C" {
	void hostShutDownAll() {
		meshes.clear();
		images.clear();
		buffers.clear();
		shaders.clear();
		wins.clear();
	}
	void hostWindowClearBackground() {
		for (auto &win : wins) {
			win->clearBackground();
		}
	}
	void hostWindowSwapBuffers() {
		for (auto &win : wins) {
			win->swapBuffer();
		}
	}
	void hostUpdateWindows() {
		for (auto &win : wins) {
			win->update();
		}
	}
}