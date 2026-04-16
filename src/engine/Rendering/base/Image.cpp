#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <SDL3/SDL_error.h>

#include "Rendering.hpp"

using namespace Rendering;

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