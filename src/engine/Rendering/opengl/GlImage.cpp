#include "GlRendering.hpp"

#include <glad/gl.h>
#include <filesystem>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_pixels.h>

using namespace OpenGl;
using namespace Rendering;

bool GlImage::hasInit = false;

GlImage::GlImage(const char *filePath)
{
	spdlog::debug("Creating image at path {}", filePath);

	if (!std::filesystem::exists(filePath)) {
		spdlog::error("Image path: {} does not exist", filePath);
		return;
	}
	SDL_Surface *surf = loadFile(filePath);
	if (!surf)
	{
		spdlog::error("Failed to load image: {}", filePath);
		return;
	}
	setupObject(surf);
	SDL_DestroySurface(surf);
}

GlImage::GlImage(GLuint existingTexture, glm::vec2 size)
{
	spdlog::debug("Creating image with pre existing texture id: 0x{:x} with size {},{}", existingTexture, size.x, size.y);
	if (existingTexture == 0)
	{
		spdlog::error("GlImage: trying to wrap invalid texture (0)");
	}

	textureID = existingTexture;
	imageSizes = size;
}

GlImage::GlImage(SDL_Surface *surface)
{
	spdlog::debug("Creating image from surface memory address 0x{:x}", reinterpret_cast<uintptr_t>(surface));
	setupObject(surface);
}

void GlImage::setupObject(SDL_Surface *surface_old)
{
	SDL_Surface* surface =
        SDL_ConvertSurface(surface_old, SDL_PIXELFORMAT_RGBA32);

    if (!surface)
    {
        spdlog::error("Convert failed: {}", SDL_GetError());
        return;
    }

    glGenTextures(1, &textureID);
    glGenSamplers(1, &samplerID);

    glBindTexture(GL_TEXTURE_2D, textureID);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        surface->w,
        surface->h,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        surface->pixels
    );

    glBindSampler(0, samplerID);

    imageSizes = glm::vec2(surface->w, surface->h);

    SDL_DestroySurface(surface);
}

GlImage::~GlImage()
{
	spdlog::debug("Deleting image with id: 0x{:x}", textureID);
	if (textureID != 0)
		glDeleteTextures(1, &textureID);
	if (samplerID != 0)
		glDeleteSamplers(1, &samplerID);
}

void GlImage::clearTransparent()
{
	if (textureID == 0) return;

	GLubyte clearColor[4] = {0,0,0,0};

	glClearTexImage(textureID, 0, GL_RGBA, GL_UNSIGNED_BYTE, clearColor);
}