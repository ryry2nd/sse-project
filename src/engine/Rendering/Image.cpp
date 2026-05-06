#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL_error.h>

#include "Rendering.hpp"

using namespace Rendering;

SDL_Surface *Image::loadFile(const std::string &filePath)
{
	SDL_Surface *surface = IMG_Load(filePath.c_str());
	if (!surface)
	{
		spdlog::error("Image load fail: {}", SDL_GetError());
		return nullptr;
	}
	return surface;
}

glm::vec2 Image::getSizes() {
	return imageSizes;
}