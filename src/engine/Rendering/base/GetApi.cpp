#include "Rendering.hpp"
#include "spdlog/spdlog.h"

#include <vector>
#include <filesystem>
#include <unordered_map>

#include <SDL3/SDL_loadso.h>

#define COMPILED_OUT_PATH "libs/Rendering"

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

using namespace Rendering;

using CreateShaderFn =
    std::unique_ptr<Shader>(*)(const char*, const char*);

using CreateMeshFn =
    std::unique_ptr<Mesh>(*)(
        Rendering::Shader*,
        const std::vector<float>&,
        const std::vector<unsigned int>&,
        const std::vector<short>&,
        const Rendering::MeshTypes&
    );

using CreateImageFromFileFn =
    std::unique_ptr<Image>(*)(const std::string&);

using CreateImageFromSurfaceFn =
    std::unique_ptr<Image>(*)(SDL_Surface*);

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

typedef std::string (*GetNameFn)();

CreateShaderFn createShaderFunc = nullptr;
CreateMeshFn createMeshFunc = nullptr;
CreateImageFromFileFn createImageFromFileFunc = nullptr;
CreateImageFromSurfaceFn createImageFromSurfaceFunc = nullptr;
CreateWindowFn createWindowFunc = nullptr;

auto sdlDeleter = [](SDL_SharedObject* obj) {
    if (obj) SDL_UnloadObject(obj);
};

using SDLSharedPtr = std::unique_ptr<SDL_SharedObject, decltype(sdlDeleter)>;

std::unordered_map<std::string, SDLSharedPtr> libs;

void createLibs() {
    namespace fs = std::filesystem;

    for (const auto& entry : fs::directory_iterator(COMPILED_OUT_PATH)) {
        if (entry.path().extension() == LIBRARY_SUFFIX) {
            SDLSharedPtr lib(SDL_LoadObject(entry.path().c_str()), sdlDeleter);

            auto getName = reinterpret_cast<GetNameFn>(SDL_LoadFunction(lib.get(), "getName"));
            if (!getName) continue;

            libs[getName()] = std::move(lib);
        }
    }
}

void CreationFunctions::initAPI(const std::string &apiName) {
    if (libs.empty()) createLibs();

    auto it = libs.find(apiName);
    if (it == libs.end()) {spdlog::error("Library not found: {}", apiName);return;}

    SDL_SharedObject *lib = it->second.get();

    createShaderFunc = (CreateShaderFn)SDL_LoadFunction(lib, "createShader");
    createMeshFunc = (CreateMeshFn)SDL_LoadFunction(lib, "createMesh");
    createImageFromFileFunc = (CreateImageFromFileFn)SDL_LoadFunction(lib, "createImageFromFile");
    createImageFromSurfaceFunc = (CreateImageFromSurfaceFn)SDL_LoadFunction(lib, "createImageFromSurface");
    createWindowFunc = (CreateWindowFn)SDL_LoadFunction(lib, "createWindow");
}

std::unique_ptr<Shader> CreationFunctions::createShader(const char* vertex, const char* fragment) {
    if (!createShaderFunc) {
        spdlog::error("createShader not loaded");
        return nullptr;
    }

    return createShaderFunc(vertex, fragment);
}
std::unique_ptr<Mesh> CreationFunctions::createMesh(Rendering::Shader *shady, const std::vector<float> &vertices, const std::vector<unsigned int> &indices, const std::vector<short> &vertLogic, const Rendering::MeshTypes &meshType) {
    if (!createMeshFunc) {
        spdlog::error("createShader not loaded");
        return nullptr;
    }

    return createMeshFunc(shady, vertices, indices, vertLogic, meshType);
}
std::unique_ptr<Image> CreationFunctions::createImage(const std::string &filePath) {
    if (!createImageFromFileFunc) {
        spdlog::error("createImage not loaded");
        return nullptr;
    }

    return createImageFromFileFunc(filePath);
}
std::unique_ptr<Image> CreationFunctions::createImage(SDL_Surface *surface) {
    if (!createImageFromSurfaceFunc) {
        spdlog::error("createImage not loaded");
        return nullptr;
    }

    return createImageFromSurfaceFunc(surface);
}
std::unique_ptr<Window> CreationFunctions::createWindow(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa, bool fullscreen, int vsync, bool hideMouse) {
    if (!createWindowFunc) {
        spdlog::error("createWindow not loaded");
        return nullptr;
    }

    return createWindowFunc(res, name, flags, aa, fullscreen, vsync, hideMouse);
}