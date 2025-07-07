#include "HelperFunctions.hpp"

#include <SDL2/SDL_image.h>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace Rendering;

HelperFunctions::HelperFunctions(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa, bool fullscreen, bool hideMouse)
{
    // it initialises sdl
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
        throw std::runtime_error("Sdl cant initialise");
    }
    if (TTF_Init() == -1)
    {
        SDL_Log("TTF_Init error: %s", TTF_GetError());
    }

    if (aa >= 0)
    {
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, aa);
    }

    window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, res.x, res.y, flags);
    if (!window)
    {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << "\n";
        SDL_Quit();
        throw std::runtime_error("cant create sdl window");
    }

    if (hideMouse)
        SDL_SetRelativeMouseMode(SDL_TRUE); // hides the mouse
    if (fullscreen)
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
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
        throw std::runtime_error("Failed to load font \"" + fontPath + "\": " + TTF_GetError());
}

Font::~Font()
{
    TTF_CloseFont(font);
}

SDL_Surface *Font::renderText(const std::string &message, SDL_Color color)
{
    if (!font)
        throw std::runtime_error("no font defined");

    SDL_Surface *surf = TTF_RenderUTF8_Blended(font, message.c_str(), color);
    if (!surf)
        throw std::runtime_error("surface failed to initialise");

    SDL_Surface *formattedSurf = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_ABGR8888, 0);
    SDL_FreeSurface(surf);
    return formattedSurf;
}

SDL_Surface *Image::loadFile(const std::string &filePath)
{
    SDL_Surface *surface = IMG_Load(filePath.c_str());
    if (!surface)
    {
        std::cerr << "Image load fail: " << IMG_GetError() << "\n";
        throw std::runtime_error("image broke");
    }
    return surface;
}