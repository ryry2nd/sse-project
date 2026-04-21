#include "Rendering.hpp"
#include <spdlog/spdlog.h>
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
        spdlog::error("Failed to load font: {}: {}", fontPath, SDL_GetError());
}

Font::~Font()
{
    TTF_CloseFont(font);
}

SDL_Surface *Font::renderText(const std::string &message, const glm::vec4 &color)
{
    if (!font) {
        spdlog::error("No Font Defined");
        return nullptr;
    }

    SDL_Surface *surf = TTF_RenderText_Blended(font, message.c_str(), message.length(), Vec4ToSDLColor(color));
    if (!surf) {
        spdlog::error("surface failed to initialise");
        return nullptr;
    }

    SDL_Surface *formattedSurf = SDL_ConvertSurface(surf, SDL_PIXELFORMAT_ABGR8888);
    SDL_DestroySurface(surf);
    return formattedSurf;
}