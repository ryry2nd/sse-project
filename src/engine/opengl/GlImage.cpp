#include "HelperFunctionsOpengl.hpp"

using namespace OpenGl;
using namespace Rendering;

ImageApi::ImageApi(const std::string &filePath)
{
    SDL_Surface *surface = IMG_Load(filePath.c_str());
    if (!surface)
    {
        std::cerr << "Image load fail: " << IMG_GetError() << "\n";
        return;
    }
    setupObject(surface);
}

ImageApi::ImageApi(SDL_Surface *surface)
{
    setupObject(surface);
}

void ImageApi::setupObject(SDL_Surface *surface)
{

    GLenum format = surface->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0,
                 format, GL_UNSIGNED_BYTE, surface->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    imageSizes = glm::vec2(surface->w, surface->h);

    SDL_FreeSurface(surface);
}

ImageApi::~ImageApi()
{
    if (textureID)
        glDeleteTextures(1, &textureID);
}

GLuint ImageApi::getID() const
{
    return textureID;
}

Image *ImageApi::makeNewImage(const std::string &filePath) const
{
    return new ImageApi(filePath);
}

Image *ImageApi::makeNewImage(SDL_Surface *surface) const
{
    return new ImageApi(surface);
}