#include "GlRendering.hpp"
#include <glad/gl.h>
#include <filesystem>
#include <iostream>

#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_pixels.h>

using namespace OpenGl;
using namespace Rendering;

GlImage::GlImage(const std::string &filePath)
{
    if (!std::filesystem::exists(filePath)) {
        std::cerr << "Image path: " << filePath << " does not exist\n";
        return;
    }
    setupObject(loadFile(filePath));
}

GlImage::GlImage(SDL_Surface *surface)
{
    setupObject(surface);
}

void GlImage::setupObject(SDL_Surface *surface)
{

    int bytesPerPixel = SDL_BYTESPERPIXEL(surface->format);
    GLenum format = (bytesPerPixel == 4) ? GL_RGBA : GL_RGB;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0,
                 format, GL_UNSIGNED_BYTE, surface->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    imageSizes = glm::vec2(surface->w, surface->h);

    SDL_DestroySurface(surface);
}

GlImage::~GlImage()
{
    if (textureID != 0)
        glDeleteTextures(1, &textureID);
}

GLuint GlImage::getID() const
{
    return textureID;
}