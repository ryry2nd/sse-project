#include "HelperFunctions.hpp"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace Rendering;

Uint64 HelperFunctions::lastCounter = SDL_GetPerformanceCounter();
double HelperFunctions::fps = 0.0f;
float HelperFunctions::deltaTime = 0.0f;
Uint64 HelperFunctions::now = SDL_GetTicks();
glm::vec2 HelperFunctions::res;
SDL_Window *HelperFunctions::window = nullptr;


SDL_Color Vec4ToSDLColor(const glm::vec4& color) {
    return SDL_Color{
        static_cast<Uint8>(color.r),
        static_cast<Uint8>(color.g),
        static_cast<Uint8>(color.b),
        static_cast<Uint8>(color.a)
    };
}

void HelperFunctions::Update()
{
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    res = glm::vec2(width, height);
    now = SDL_GetTicks();
    Uint64 currentCounter = SDL_GetPerformanceCounter();
    deltaTime = static_cast<float>(currentCounter - lastCounter) / SDL_GetPerformanceFrequency();
    fps = SDL_GetPerformanceFrequency() / static_cast<double>(currentCounter - lastCounter);
    lastCounter = currentCounter;
}

HelperFunctions::HelperFunctions(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa, bool fullscreen, bool hideMouse)
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

HelperFunctions::~HelperFunctions()
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