#include "GlRendering.hpp"

#include <spdlog/spdlog.h>
#include <glad/gl.h>
#include <filesystem>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_pixels.h>

using namespace OpenGl;
using namespace Rendering;

GlImage::GlImage(const std::string &filePath)
{
    if (!std::filesystem::exists(filePath)) {
        spdlog::error("Image path: {} does not exist", filePath);
        return;
    }
    setupObject(loadFile(filePath));
}

GlImage::GlImage(GLuint existingTexture, glm::vec2 size, bool ownsTexture)
{
    if (existingTexture == 0)
    {
        spdlog::error("GlImage: trying to wrap invalid texture (0)");
    }
    
    textureID = existingTexture;
    imageSizes = size;
    this->ownsTexture = ownsTexture;
}

GlImage::GlImage(SDL_Surface *surface)
{
    setupObject(surface);
}

void GlImage::setupObject(SDL_Surface *surface)
{

    int bytesPerPixel = SDL_BYTESPERPIXEL(surface->format);

    GLenum format = (bytesPerPixel == 4) ? GL_RGBA : GL_RGB;
    GLenum internalFormat = (bytesPerPixel == 4) ? GL_RGBA8 : GL_RGB8;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0,
                internalFormat,
                surface->w, surface->h, 0,
                format,
                GL_UNSIGNED_BYTE,
                surface->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glGenerateMipmap(GL_TEXTURE_2D);

    imageSizes = glm::vec2(surface->w, surface->h);

    SDL_DestroySurface(surface);
}

GlImage::~GlImage()
{
    if (textureID != 0 && ownsTexture)
        glDeleteTextures(1, &textureID);
}

GLuint GlImage::getID() const
{
    return textureID;
}