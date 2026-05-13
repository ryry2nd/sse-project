#include <glm/ext/vector_float2.hpp>
#include <engine/Rendering/Rendering.hpp>
#include <memory>
#include <unordered_map>

using namespace Rendering;

typedef uint32_t Uint32;
typedef uint64_t Uint64;
typedef uint8_t Uint8;

std::unordered_map<std::string, std::unique_ptr<Rendering::Window>> wins;
std::unordered_map<std::string, std::unique_ptr<Rendering::Shader>> shaders;
std::unordered_map<std::string, std::unique_ptr<Rendering::Buff>> buffers;
std::unordered_map<std::string, std::unique_ptr<Rendering::Mesh>> meshes;
std::unordered_map<std::string, std::unique_ptr<Rendering::Image>> images;

Rendering::Window *cacheWin;
Rendering::Buff *cacheBuff;
Rendering::Mesh *cacheMesh;
Rendering::Image *cacheImage;

extern "C" {
	void hostShutDownAll() {
		cacheWin = nullptr;
		wins.clear();
		shaders.clear();
		cacheBuff = nullptr;
		buffers.clear();
		cacheMesh = nullptr;
		meshes.clear();
		cacheImage = nullptr;
		images.clear();
	}


	void hostSetAPI(const char* api) {
		CreationFunctions::initAPI(api);
	}
	void hostCreateWindow(const char *name, glm::vec2 res, const char *winName, Uint32 flags, Uint32 aa=0, bool fullscreen=false, int vsync = 0, bool hideMouse = true) {
		auto win = CreationFunctions::createWindow(res, winName, flags, aa, fullscreen, vsync, hideMouse);
		cacheWin = win.get();
		wins[name] = std::move(win);
	}
	void hostSetWindow(const char *name) {
		cacheWin = wins[name].get();
	}

	void hostWindowClearBackground() {
		for (auto &win : wins) {
			win.second->clearBackground();
		}
	}
	void hostWindowUpdateScreenRes() {
		cacheWin->updateScreenRes();
	}
	void hostWindowSwapBuffers() {
		for (auto &win : wins) {
			win.second->swapBuffer();
		}
	}
	void hostWindowSetBackgroundColor(glm::vec4 color) {
		cacheWin->setBackgroundColor(color);
	}
	float hostGetWindowDeltaTime() {
		return cacheWin->deltaTime;
	}
	float hostGetWindowFPS() {
		return cacheWin->fps;
	}
	glm::vec2 hostGetWindowRes() {
		return cacheWin->res;
	}
	void hostWindowDisableDepthTest() {
		cacheWin->disableDepthTest();
	}
	void hostWindowEnableDepthTest() {
		cacheWin->enableDepthTest();
	}
	void hostWindowEnableBackfaceCull() {
		cacheWin->enableBackfaceCull();
	}
	void hostWindowDisableBackfaceCull() {
		cacheWin->disableBackfaceCull();
	}
	void hostUpdateWindows() {
		for (auto &win : wins) {
			win.second->update();
		}
	}
	const bool *hostWindowGetKeystates(int &numKeys) {
		return Window::getKeystates(numKeys);
	}



	void hostCreateShader(const char *name, const char *path) {
		auto shdr = CreationFunctions::createShader(path);
		shaders[name] = std::move(shdr);
	}



	void hostCreateBuffer(const char *name, Buff::Type type, Buff::Frequency freq, std::size_t size, const void* data = nullptr) {
		auto buff = CreationFunctions::createBuff(type, freq, size, data);
		cacheBuff = buff.get();
		buffers[name] = std::move(buff);
	}
	void hostSetBuffer(const char *name) {
		cacheBuff = buffers[name].get();
	}
	std::size_t hostBuffGetSize() {
		return cacheBuff->getSize();
	}
	Buff::Type hostBuffGetType() {
		return cacheBuff->getType();
	}
	Buff::Frequency hostBuffGetFrequency() {
		return cacheBuff->getFrequency();
	}
	void hostBuffWrite(const std::size_t offset, const std::size_t size, const void* data) {
		cacheBuff->write(offset, size, data);
	}
	void hostBuffRead(const std::size_t offset, const std::size_t size, void* data) {
		cacheBuff->read(offset, size, data);
	}



	void hostCreateMesh(const char *name, const float *vertices, const size_t vert_size, const unsigned int *indices, const size_t ind_size, const short *vertLogic, const size_t vert_logic_size, Rendering::Mesh::MeshTypes meshType = Rendering::Mesh::MeshTypes::Triangles) {
		auto mesh = CreationFunctions::createMesh(vertices, vert_size, indices, ind_size, vertLogic, vert_logic_size, meshType);
		cacheMesh = mesh.get();
		meshes[name] = std::move(mesh);
	}
	void hostSetMesh(const char *name) {
		cacheMesh = meshes[name].get();
	}
	Mesh::MeshTypes hostGetMeshType() {
		return cacheMesh->getMeshType();
	}
	void hostSetMeshType(Mesh::MeshTypes meshType) {
		cacheMesh->setMeshType(meshType);
	}
	bool hostMeshGetBackFaceCulling() {
		return cacheMesh->getBackFaceCulling();
	}
	void hostMeshToggleBackFaceCulling() {
		cacheMesh->toggleBackFaceCulling();
	}



	void hostCreateImage(const char *name, const char *filePath) {
		auto img = CreationFunctions::createImage(filePath);
		cacheImage = img.get();
		images[name] = std::move(img);
	}
	void hostCreateImage2(const char *name, SDL_Surface *surface) {
		auto img = CreationFunctions::createImage(surface);
		cacheImage = img.get();
		images[name] = std::move(img);
	}
	void hostSetImage(const char *name) {
		cacheImage = images[name].get();
	}
	glm::vec2 hostGetImageSizes() {
		return cacheImage->getSizes();
	}
	void hostImageClearTransparent() {
		cacheImage->clearTransparent();
	}
}