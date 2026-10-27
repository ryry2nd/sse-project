#include <filesystem>
#include <unordered_map>
#include "apis/internal/dlopen.hpp"
#include <spdlog/spdlog.h>

#include "apis/internal/ApiFunction.hpp"


#define COMPILED_OUT_PATH "lib/Rendering"

using namespace Engine::Rendering;

using namespace NApiFunctions;

APIFunctions apiFunctions;

std::unordered_map<std::string, void *> libs;

void createLibs() {
	namespace fs = std::filesystem;

	for (const auto& entry : fs::directory_iterator(COMPILED_OUT_PATH)) {
		if (entry.path().extension() == LIBRARY_SUFFIX) {
			void* handle = initruntime(entry.path().string().c_str(), false);

			if (!handle)
			{
				spdlog::warn("Failed to load {}", entry.path().string());
				continue;
			}

			auto getName = reinterpret_cast<GetNameFn>(getFunction(handle, "getName"));
			if (!getName) continue;

			libs[getName()] = handle;
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

		void *lib = it->second;

		apiFunctions.createShaderFunc = (CreateShaderFn)getFunction(lib, "createShader");
		apiFunctions.createMeshFunc = (CreateMeshFn)getFunction(lib, "createMesh");
		apiFunctions.createImageFromFileFunc = (CreateImageFromFileFn)getFunction(lib, "createImageFromFile");
		apiFunctions.createImageFromSurfaceFunc = (CreateImageFromSurfaceFn)getFunction(lib, "createImageFromSurface");
		apiFunctions.createImageFromSizeFunc = (CreateImageFromSizeFn)getFunction(lib, "createImageFromSize");
		apiFunctions.createWindowFunc = (CreateWindowFn)getFunction(lib, "createWindow");
		apiFunctions.createBuffFunc = (CreateBuffFn)getFunction(lib, "createBuff");
		apiFunctions.createDrawFunc = (CreateDrawFn)getFunction(lib, "draw");
		apiFunctions.createDrawCompFunc = (CreateDrawCompFn)getFunction(lib, "computeCall");
		apiFunctions.createFboFunc = (CreateFBOFn)getFunction(lib, "createFrameBuffer");

		if (!apiFunctions.createShaderFunc ||
			!apiFunctions.createMeshFunc ||
			!apiFunctions.createImageFromFileFunc ||
			!apiFunctions.createWindowFunc ||
			!apiFunctions.createBuffFunc ||
			!apiFunctions.createDrawFunc ||
			!apiFunctions.createFboFunc ||
			!apiFunctions.createDrawCompFunc ||
			!apiFunctions.createImageFromSizeFunc
		)
		{
			spdlog::warn("One of the creation functions in the {} api. If you call it you will segfault", apiName);
		}

		spdlog::info("Successfully set API to {}", apiName);
	}
	void deinitAPI() {
		for (auto &lib : libs) {
			rmruntime(lib.second);
		}
		libs.clear();
	}
}

APIFunctions *NApiFunctions::getApiFunctions() {
	return &apiFunctions;
}