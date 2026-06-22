#include "Rendering.hpp"

#include <memory>
#include <filesystem>
#include <unordered_map>
#include <SDL3/SDL_loadso.h>
#include <spdlog/spdlog.h>

#include "internal/Internal.hpp"

#define COMPILED_OUT_PATH "lib/Rendering"

#ifdef _WIN32
#define POPEN _popen
#define PCLOSE _pclose
#define EXE_EXT ".exe"
#define BIN_PREFIX ""
#define LIBRARY_SUFFIX ".dll"
#else
#define POPEN popen
#define PCLOSE pclose
#define EXE_EXT ""
#define BIN_PREFIX "./"
#define LIBRARY_SUFFIX ".so"
#endif

using namespace Engine::Rendering;

using CreateShaderFn =
	std::unique_ptr<Shader>(*)(std::string);

using CreateMeshFn =
	std::unique_ptr<Mesh>(*)(
		const Vert*,
		const size_t,
		const Ind*,
		const size_t
	);

using CreateImageFromFileFn =
	std::unique_ptr<Image>(*)(const char*, ImageSettings);

using CreateImageFromSurfaceFn =
	std::unique_ptr<Image>(*)(SDL_Surface*, ImageSettings);

using CreateWindowFn =
	std::unique_ptr<Window>(*)(
		glm::vec2,
		const char*,
		Uint32,
		Uint32,
		bool,
		int,
		bool
	);

using CreateBuffFn =
	std::unique_ptr<Buff>(*)(
		Buff::Type,
		Buff::Frequency,
		std::size_t,
		const void*
	);

using CreateDrawFn =
	void(*)(
		Window*,
		Mesh*,
		InternalParams*,
		size_t
	);

using CreateFBOFn =
	std::unique_ptr<FrameBuffer>(*)(
		glm::vec2 size,
		uint32_t settings
	);

typedef const char *(*GetNameFn)();

struct APIFunctions {
	CreateShaderFn createShaderFunc = nullptr;
	CreateMeshFn createMeshFunc = nullptr;
	CreateImageFromFileFn createImageFromFileFunc = nullptr;
	CreateImageFromSurfaceFn createImageFromSurfaceFunc = nullptr;
	CreateWindowFn createWindowFunc = nullptr;
	CreateBuffFn createBuffFunc = nullptr;
	CreateDrawFn createDrawFunc = nullptr;
	CreateFBOFn createFboFunc = nullptr;
};
APIFunctions apiFunctions;

auto sdlDeleter = [](SDL_SharedObject* obj) {
	if (obj) SDL_UnloadObject(obj);
};

using SDLSharedPtr = std::unique_ptr<SDL_SharedObject, decltype(sdlDeleter)>;

std::unordered_map<std::string, SDLSharedPtr> libs;

void createLibs() {
	namespace fs = std::filesystem;

	for (const auto& entry : fs::directory_iterator(COMPILED_OUT_PATH)) {
		if (entry.path().extension() == LIBRARY_SUFFIX) {
			SDL_SharedObject* raw = SDL_LoadObject(entry.path().string().c_str());

			if (!raw)
			{
				spdlog::warn("Failed to load {}: {}", entry.path().string(), SDL_GetError());
				continue;
			}

			SDLSharedPtr lib(raw, sdlDeleter);

			auto getName = reinterpret_cast<GetNameFn>(SDL_LoadFunction(lib.get(), "getName"));
			if (!getName) continue;

			libs[getName()] = std::move(lib);
		}
	}

	std::string deb = "";

	for (const auto& [key, value] : libs)
	{
		deb += key + " ";
	}

	if (deb == "") {
		spdlog::critical("No rendering APIs found");
	}
	else {
		spdlog::info("Lib choice(s): {}", deb);
	}
}

void Internal::Functions::initAPI(const char *apiName) {
	if (libs.empty()) createLibs();

	auto it = libs.find(apiName);
	if (it == libs.end()) {
		spdlog::critical("Library not found: {}", apiName);
		std::exit(1);
	}

	SDL_SharedObject *lib = it->second.get();

	apiFunctions.createShaderFunc = (CreateShaderFn)SDL_LoadFunction(lib, "createShader");
	apiFunctions.createMeshFunc = (CreateMeshFn)SDL_LoadFunction(lib, "createMesh");
	apiFunctions.createImageFromFileFunc = (CreateImageFromFileFn)SDL_LoadFunction(lib, "createImageFromFile");
	apiFunctions.createImageFromSurfaceFunc = (CreateImageFromSurfaceFn)SDL_LoadFunction(lib, "createImageFromSurface");
	apiFunctions.createWindowFunc = (CreateWindowFn)SDL_LoadFunction(lib, "createWindow");
	apiFunctions.createBuffFunc = (CreateBuffFn)SDL_LoadFunction(lib, "createBuff");
	apiFunctions.createDrawFunc = (CreateDrawFn)SDL_LoadFunction(lib, "draw");
	apiFunctions.createFboFunc = (CreateFBOFn)SDL_LoadFunction(lib, "createFrameBuffer");

	if (!apiFunctions.createShaderFunc ||
		!apiFunctions.createMeshFunc ||
		!apiFunctions.createImageFromFileFunc ||
		!apiFunctions.createWindowFunc ||
		!apiFunctions.createBuffFunc ||
		!apiFunctions.createDrawFunc ||
		!apiFunctions.createFboFunc
	)
	{
		spdlog::warn("One of the creation functions in the {} api. If you call it you will segfault", apiName);
	}

	spdlog::info("Successfully set API to {}", apiName);
}

std::unique_ptr<Shader> Internal::Functions::createShader(const char *path) {
	return apiFunctions.createShaderFunc(path);
}
std::unique_ptr<Mesh> Internal::Functions::createMesh(const Vert *vertices, const size_t vert_size, const Ind *indices, const size_t ind_size) {
	return apiFunctions.createMeshFunc(vertices, vert_size, indices, ind_size);
}
std::unique_ptr<Image> Internal::Functions::createImage(const char *filePath, ImageSettings settings) {
	return apiFunctions.createImageFromFileFunc(filePath, settings);
}
std::unique_ptr<Image> Internal::Functions::createImage(SDL_Surface *surface, ImageSettings settings) {
	return apiFunctions.createImageFromSurfaceFunc(surface, settings);
}
std::unique_ptr<Window> Internal::Functions::createWindow(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa, bool fullscreen, int vsync, bool hideMouse) {
	return apiFunctions.createWindowFunc(res, name, flags, aa, fullscreen, vsync, hideMouse);
}

std::unique_ptr<Buff> Internal::Functions::createBuff(Buff::Type type, Buff::Frequency freq, std::size_t size, const void* data) {
	return apiFunctions.createBuffFunc(type, freq, size, data);
}

std::unique_ptr<FrameBuffer> Internal::Functions::createFrameBuffer(glm::vec2 size, uint32_t settings) {
	return apiFunctions.createFboFunc(size, settings);
}

void Internal::Functions::draw(Window *win, Mesh *mesh, InternalParams *params, size_t size) {
	return apiFunctions.createDrawFunc(win, mesh, params, size);
}