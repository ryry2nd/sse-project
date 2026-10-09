#include <memory>
#include <filesystem>
#include <unordered_map>
#include <SDL3/SDL_loadso.h>
#include <spdlog/spdlog.h>

#include "ApiFunction.hpp"


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

using namespace NApiFunctions;

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

extern "C" {
	void initAPI(const char *apiName) {
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
}

APIFunctions *NApiFunctions::getApiFunctions() {
	return &apiFunctions;
}