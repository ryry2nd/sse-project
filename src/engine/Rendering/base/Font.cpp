#include "Rendering.hpp"
#include <SDL3_ttf/SDL_ttf.h>


using namespace Rendering;

void Font::init() {
    if (TTF_Init() == 0)
    {
        SDL_Log("TTF_Init error: %s", SDL_GetError());
    }
}

void Font::deinit() {
    TTF_Quit();
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