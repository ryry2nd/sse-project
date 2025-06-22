#include "HelperFunctionsOpengl.hpp"

ImageOpenGl::ImageOpenGl(const std::string &filePath)
{
    SDL_Surface *surface = IMG_Load(filePath.c_str());
    if (!surface)
    {
        std::cerr << "Image load fail: " << IMG_GetError() << "\n";
        return;
    }

    GLenum format = surface->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0,
                 format, GL_UNSIGNED_BYTE, surface->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    SDL_FreeSurface(surface);
}

ImageOpenGl::~ImageOpenGl()
{
    if (textureID)
        glDeleteTextures(1, &textureID);
}

GLuint ImageOpenGl::getID() const
{
    return textureID;
}

Image *ImageOpenGl::makeNewImage(const std::string &filePath) const
{
    return new ImageOpenGl(filePath);
}