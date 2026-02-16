#include "HelperFunctions.hpp"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>
#include <unordered_map>

using namespace Rendering;

Uint64 Window::lastCounter = SDL_GetPerformanceCounter();
double Window::fps = 0.0f;
float Window::deltaTime = 0.0f;
Uint64 Window::now = SDL_GetTicks();
glm::vec2 Window::res;
SDL_Window *Window::window = nullptr;


SDL_Color Vec4ToSDLColor(const glm::vec4& color) {
    return SDL_Color{
        static_cast<Uint8>(color.r),
        static_cast<Uint8>(color.g),
        static_cast<Uint8>(color.b),
        static_cast<Uint8>(color.a)
    };
}

void Window::Update()
{
    SDL_PumpEvents();
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    res = glm::vec2(width, height);
    now = SDL_GetTicks();
    Uint64 currentCounter = SDL_GetPerformanceCounter();
    deltaTime = static_cast<float>(currentCounter - lastCounter) / SDL_GetPerformanceFrequency();
    fps = SDL_GetPerformanceFrequency() / static_cast<double>(currentCounter - lastCounter);
    lastCounter = currentCounter;
}

const bool *Window::getKeystates(int &numKeys) {
    return SDL_GetKeyboardState(&numKeys);
}

Window::Window(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa, bool fullscreen, bool hideMouse)
{
    // it initialises sdl
    if (SDL_Init(SDL_INIT_VIDEO) == 0)
    {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
        throw std::runtime_error("Sdl cant initialise");
    }
    if (TTF_Init() == 0)
    {
        SDL_Log("TTF_Init error: %s", SDL_GetError());
    }

    if (aa > 0)
    {
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, aa);
    }

    window = SDL_CreateWindow(name, res.x, res.y, flags);
    if (!window)
    {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << "\n";
        SDL_Quit();
        throw std::runtime_error("cant create sdl window");
    }

    if (hideMouse)
        // hides the mouse
        SDL_SetWindowRelativeMouseMode(window, true);
    if (fullscreen) {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
        SDL_SetWindowResizable(window, true);
        SDL_SetWindowBordered(window, false);
    }
}

Window::~Window()
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}

Font::Font(const std::string &fontPath, int size)
{
    font = TTF_OpenFont(fontPath.c_str(), size);
    if (!font)
        throw std::runtime_error("Failed to load font \"" + fontPath + "\": " + SDL_GetError());
}

Font::~Font()
{
    TTF_CloseFont(font);
}

SDL_Surface *Font::renderText(const std::string &message, const glm::vec4 &color)
{
    if (!font)
        throw std::runtime_error("no font defined");

    SDL_Surface *surf = TTF_RenderText_Blended(font, message.c_str(), message.length(), Vec4ToSDLColor(color));
    if (!surf)
        throw std::runtime_error("surface failed to initialise");

    SDL_Surface *formattedSurf = SDL_ConvertSurface(surf, SDL_PIXELFORMAT_ABGR8888);
    SDL_DestroySurface(surf);
    return formattedSurf;
}

SDL_Surface *Image::loadFile(const std::string &filePath)
{
    SDL_Surface *surface = IMG_Load(filePath.c_str());
    if (!surface)
    {
        std::cerr << "Image load fail: " << SDL_GetError() << "\n";
        throw std::runtime_error("image broke");
    }
    return surface;
}

#define COMPILED_OUT_PATH "libs/rendering"

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
    if (it == libs.end()) throw std::runtime_error("Library not found: " + apiName);

    SDL_SharedObject *lib = it->second.get();

    createShaderFunc = (CreateShaderFn)SDL_LoadFunction(lib, "createShader");
    createMeshFunc = (CreateMeshFn)SDL_LoadFunction(lib, "createMesh");
    createImageFromFileFunc = (CreateImageFromFileFn)SDL_LoadFunction(lib, "createImageFromFile");
    createImageFromSurfaceFunc = (CreateImageFromSurfaceFn)SDL_LoadFunction(lib, "createImageFromSurface");
    createWindowFunc = (CreateWindowFn)SDL_LoadFunction(lib, "createWindow");
}

std::unique_ptr<Shader> CreationFunctions::createShader(const char* vertex, const char* fragment) {
    if (!createShaderFunc)
    throw std::runtime_error("createShader not loaded");

    return createShaderFunc(vertex, fragment);
}
std::unique_ptr<Mesh> CreationFunctions::createMesh(Rendering::Shader *shady, const std::vector<float> &vertices, const std::vector<unsigned int> &indices, const std::vector<short> &vertLogic, const Rendering::MeshTypes &meshType) {
    if (!createMeshFunc)
    throw std::runtime_error("createShader not loaded");

    return createMeshFunc(shady, vertices, indices, vertLogic, meshType);
}
std::unique_ptr<Image> CreationFunctions::createImage(const std::string &filePath) {
    if (!createImageFromFileFunc)
    throw std::runtime_error("createShader not loaded");

    return createImageFromFileFunc(filePath);
}
std::unique_ptr<Image> CreationFunctions::createImage(SDL_Surface *surface) {
    if (!createImageFromSurfaceFunc)
    throw std::runtime_error("createShader not loaded");

    return createImageFromSurfaceFunc(surface);
}
std::unique_ptr<Window> CreationFunctions::createWindow(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa, bool fullscreen, int vsync, bool hideMouse) {
    if (!createWindowFunc)
    throw std::runtime_error("createShader not loaded");

    return createWindowFunc(res, name, flags, aa, fullscreen, vsync, hideMouse);
}