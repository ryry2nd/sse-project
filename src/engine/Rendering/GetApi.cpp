#include "Rendering.hpp"

#include <memory>
#include <spdlog/spdlog.h>
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
        const float*,
        const size_t,
        const unsigned int*,
        const size_t,
        const short*,
        const size_t,
        Rendering::Mesh::MeshTypes
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

using CreateBuffFn =
    std::unique_ptr<Buff>(*)(
        Buff::Type,
        Buff::Frequency,
        std::size_t,
        const void*
    );

using CreateDrawFn = 
    void(*)(
        Rendering::Material*,
        Rendering::Mesh*,
        Rendering::DrawParams*
    );

using CreateFBOFn = 
    std::unique_ptr<FrameBuffer>(*)(
        glm::vec2 size, 
        uint32_t settings
    );

typedef std::string (*GetNameFn)();

CreateShaderFn createShaderFunc = nullptr;
CreateMeshFn createMeshFunc = nullptr;
CreateImageFromFileFn createImageFromFileFunc = nullptr;
CreateImageFromSurfaceFn createImageFromSurfaceFunc = nullptr;
CreateWindowFn createWindowFunc = nullptr;
CreateBuffFn createBuffFunc = nullptr;
CreateDrawFn createDrawFunc = nullptr;
CreateFBOFn createFboFunc = nullptr;

auto sdlDeleter = [](SDL_SharedObject* obj) {
    if (obj) SDL_UnloadObject(obj);
};

using SDLSharedPtr = std::unique_ptr<SDL_SharedObject, decltype(sdlDeleter)>;

std::unordered_map<std::string, SDLSharedPtr> libs;

void createLibs() {
    namespace fs = std::filesystem;

    for (const auto& entry : fs::directory_iterator(COMPILED_OUT_PATH)) {
        if (entry.path().extension() == LIBRARY_SUFFIX) {
            SDL_SharedObject* raw = SDL_LoadObject(entry.path().c_str());

            if (!raw)
            {
                spdlog::error("Failed to load {}: {}", entry.path().string(), SDL_GetError());
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
    createBuffFunc = (CreateBuffFn)SDL_LoadFunction(lib, "createBuff");
    createDrawFunc = (CreateDrawFn)SDL_LoadFunction(lib, "draw");
    createFboFunc = (CreateFBOFn)SDL_LoadFunction(lib, "createFrameBuffer");

    spdlog::info("Successfully set API to {}", apiName);
}

std::unique_ptr<Shader> CreationFunctions::createShader(const char* vertex, const char* fragment) {
    if (!createShaderFunc) {
        spdlog::error("createShader not loaded");
        return nullptr;
    }

    spdlog::debug("created shader with paths: \n{}\n{}", vertex, fragment);
    return createShaderFunc(vertex, fragment);
}
std::unique_ptr<Mesh> CreationFunctions::createMesh(const float *vertices, const size_t vert_size, const unsigned int *indices, const size_t ind_size, const short *vertLogic, const size_t vert_logic_size, Rendering::Mesh::MeshTypes meshType) {
    if (!createMeshFunc) {
        spdlog::error("createShader not loaded");
        return nullptr;
    }

    spdlog::debug("created mesh with {} vertices, and {} indicies.", vert_size, ind_size);
    return createMeshFunc(vertices, vert_size, indices, ind_size, vertLogic, vert_logic_size, meshType);
}
std::unique_ptr<Image> CreationFunctions::createImage(const std::string &filePath) {
    if (!createImageFromFileFunc) {
        spdlog::error("createImage not loaded");
        return nullptr;
    }

    spdlog::debug("created image at path {}", filePath);
    return createImageFromFileFunc(filePath);
}
std::unique_ptr<Image> CreationFunctions::createImage(SDL_Surface *surface) {
    if (!createImageFromSurfaceFunc) {
        spdlog::error("createImage not loaded");
        return nullptr;
    }

    spdlog::debug("created image with surf memory id of {}", (void*)surface);
    return createImageFromSurfaceFunc(surface);
}
std::unique_ptr<Window> CreationFunctions::createWindow(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa, bool fullscreen, int vsync, bool hideMouse) {
    if (!createWindowFunc) {
        spdlog::error("createWindow not loaded");
        return nullptr;
    }

    spdlog::debug("created window with resolution: {} by {}", res.x, res.y);
    return createWindowFunc(res, name, flags, aa, fullscreen, vsync, hideMouse);
}

std::unique_ptr<Buff> CreationFunctions::createBuff(Buff::Type type, Buff::Frequency freq, std::size_t size, const void* data) {
    if (!createBuffFunc) {
        spdlog::error("createBuff not loaded");
        return nullptr;
    }

    spdlog::debug("created buffer with size: {}", size);
    return createBuffFunc(type, freq, size, data);
}

std::unique_ptr<FrameBuffer> CreationFunctions::createFrameBuffer(glm::vec2 size, uint32_t settings) {
    if (!createFboFunc) {
        spdlog::error("createFrameBuffer not loaded");
        return nullptr;
    }

    spdlog::debug("created frame buffer with size: {},{}", size.x, size.y);
    return createFboFunc(size, settings);
}

void CreationFunctions::draw(Material *mat, Mesh *mesh, DrawParams *params) {
    if (!createDrawFunc) {
        spdlog::error("draw not loaded");
        return;
    }

    return createDrawFunc(mat, mesh, params);
}