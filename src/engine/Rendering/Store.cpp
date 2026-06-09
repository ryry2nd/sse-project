#include "Rendering.hpp"
#include "internal/Internal.hpp"

using namespace Engine::Rendering;

std::vector<std::unique_ptr<Window>> wins;
std::vector<std::unique_ptr<Shader>> shaders;
std::vector<std::unique_ptr<Buff>> buffers;
std::vector<std::unique_ptr<Mesh>> meshes;
std::vector<std::unique_ptr<Image>> images;
std::vector<std::unique_ptr<FrameBuffer>> fbos;



void CreationFunctions::draw(Window *win, Mesh *mesh, DrawParams *params, size_t size) {
	Internal::Functions::draw(win, mesh, params, size);
}


void CreationFunctions::initAPI(const char* api) {
	Internal::Functions::initAPI(api);
}

Window *CreationFunctions::createWindow(glm::vec2 res, const char *winName, Uint32 flags, Uint32 aa, bool fullscreen, int vsync, bool hideMouse) {
	auto win = Internal::Functions::createWindow(res, winName, flags, aa, fullscreen, vsync, hideMouse);
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

Mesh *CreationFunctions::createMesh(const Vert *vertices, const size_t vert_size, const Ind *indices, const size_t ind_size) {
	auto mesh = Internal::Functions::createMesh(vertices, vert_size, indices, ind_size);
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
	auto shdr = Internal::Functions::createShader(path);
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
	auto img = Internal::Functions::createImage(filePath);
	auto ret = img.get();
	images.push_back(std::move(img));
	return ret;
}
Image *CreationFunctions::createImage(SDL_Surface *surface) {
	auto img = Internal::Functions::createImage(surface);
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
	auto buff = Internal::Functions::createBuff(type, freq, size, data);
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
	auto fbo = Internal::Functions::createFrameBuffer(size, settings);
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
		fbos.clear();
		images.clear();
		buffers.clear();
		shaders.clear();
		wins.clear();
	}
}