
#include "Image.hpp"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_surface.h>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <cstring>
#include <cstdlib>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace Engine::Rendering;

SDL_Surface* MakeSurfaceFromRGBA(
    const unsigned char* pixels,
    int w,
    int h)
{
    SDL_Surface* surface = SDL_CreateSurface(
        w,
        h,
        SDL_PIXELFORMAT_RGBA32
    );

    if (!surface)
        return nullptr;

    std::memcpy(surface->pixels, pixels, w*h*4);

    return surface;
}

SDL_Surface* LoadImage(const char* path)
{
    int width = 0;
    int height = 0;
    int channels = 0;

    // Force RGBA output, regardless of the source image format.
    unsigned char* pixels = stbi_load(
        path,
        &width,
        &height,
        &channels,
        STBI_rgb_alpha
    );

    if (!pixels)
    {
        spdlog::error(
            "Failed to load image '{}': {}",
            path,
            stbi_failure_reason()
                ? stbi_failure_reason()
                : "Unknown stb_image error"
        );

        return nullptr;
    }

    SDL_Surface* surface = MakeSurfaceFromRGBA(
        pixels,
        width,
        height
    );

    stbi_image_free(pixels);

    if (!surface)
    {
        spdlog::error(
            "Failed to create SDL surface for '{}': {}",
            path,
            SDL_GetError()
        );
    }

    return surface;
}

SDL_Surface* getErrorTex()
{
    unsigned char pixels[] = {
        0,   0,   0,   255,  // Black
        255, 0,   255, 255,  // Purple
        255, 0,   255, 255,  // Purple
        0,   0,   0,   255   // Black
    };

    return MakeSurfaceFromRGBA(pixels, 2, 2);
}

void Image::loadFile(
    const char* filePath,
    SDL_Surface*& surface,
    ImageSettings* settings)
{
	if (!filePath) {
		surface = getErrorTex();

        *settings = ImageSettings::err();
		return;
	}

    std::filesystem::path file = filePath;

    if (std::filesystem::exists(file))
    {
        // stb_image detects the image format itself.
        surface = LoadImage(filePath);

        if (!surface)
            std::exit(1);
    }
    else
    {
        spdlog::error(
            "Image path '{}' does not exist, falling back",
            filePath
        );

        surface = getErrorTex();

        *settings = ImageSettings::err();
    }
}

void Image::saveToBmp(SDL_Surface* surface, const char* path)
{
    if (!SDL_SaveBMP(surface, path))
    {
        spdlog::error(
            "Failed to save BMP '{}': {}",
            path,
            SDL_GetError()
        );
    }
}
