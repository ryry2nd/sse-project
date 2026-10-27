#include "GlRendering.hpp"

#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_pixels.h>
#include <memory>

GLenum toGL(ImageSettings::Filter f)
{
    switch (f)
    {
        case ImageSettings::Filter::Nearest: return GL_NEAREST;
        case ImageSettings::Filter::Linear:  return GL_LINEAR;

        case ImageSettings::Filter::NearestMipmapNearest: return GL_NEAREST_MIPMAP_NEAREST;
        case ImageSettings::Filter::LinearMipmapNearest:   return GL_LINEAR_MIPMAP_NEAREST;
        case ImageSettings::Filter::NearestMipmapLinear:   return GL_NEAREST_MIPMAP_LINEAR;
        case ImageSettings::Filter::LinearMipmapLinear:    return GL_LINEAR_MIPMAP_LINEAR;
    }
    return GL_LINEAR;
}

GLenum toGLNoMip(ImageSettings::Filter f)
{
    switch (f)
    {
		case ImageSettings::Filter::NearestMipmapNearest:
		case ImageSettings::Filter::NearestMipmapLinear:
        case ImageSettings::Filter::Nearest:
			return GL_NEAREST;

		case ImageSettings::Filter::LinearMipmapNearest:
        case ImageSettings::Filter::LinearMipmapLinear:
        case ImageSettings::Filter::Linear:
			return GL_LINEAR;
    }
    return GL_LINEAR;
}

GLenum toGL(ImageSettings::WrapMode w)
{
    switch (w)
    {
        case ImageSettings::WrapMode::Repeat:         return GL_REPEAT;
        case ImageSettings::WrapMode::ClampToEdge:    return GL_CLAMP_TO_EDGE;
        case ImageSettings::WrapMode::ClampToBorder:  return GL_CLAMP_TO_BORDER;
        case ImageSettings::WrapMode::MirroredRepeat: return GL_MIRRORED_REPEAT;
    }
    return GL_REPEAT;
}

GLenum toGL(ImageSettings::CompareMode m)
{
    switch (m)
    {
        case ImageSettings::CompareMode::None: return GL_NONE;
        case ImageSettings::CompareMode::CompareRefToTexture: return GL_COMPARE_REF_TO_TEXTURE;
    }
    return GL_NONE;
}

GLenum toGL(ImageSettings::CompareFunc f)
{
    switch (f)
    {
        case ImageSettings::CompareFunc::LessEqual:    return GL_LEQUAL;
        case ImageSettings::CompareFunc::GreaterEqual: return GL_GEQUAL;
        case ImageSettings::CompareFunc::Less:         return GL_LESS;
        case ImageSettings::CompareFunc::Greater:      return GL_GREATER;
        case ImageSettings::CompareFunc::Equal:        return GL_EQUAL;
        case ImageSettings::CompareFunc::NotEqual:     return GL_NOTEQUAL;
        case ImageSettings::CompareFunc::Always:       return GL_ALWAYS;
        case ImageSettings::CompareFunc::Never:        return GL_NEVER;
    }
    return GL_LEQUAL;
}

void GlImage::setupObject(SDL_Surface *surface_old, ImageSettings settings)
{
	SDL_Surface* surface = SDL_ConvertSurface(surface_old, SDL_PIXELFORMAT_RGBA32);
	// SDL_Surface *surface = surface_old;

    if (!surface)
    {
		spdlog::critical("Convert failed: {}", SDL_GetError());
		std::exit(1);
    }

	glGenTextures(1, &textureID);
	glGenSamplers(1, &samplerID);

	glBindTexture(GL_TEXTURE_2D, textureID);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

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


	if (settings.useMip) {
		useMip = true;
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, settings.baseLevel);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, settings.maxLevel);

		glSamplerParameteri(samplerID, GL_TEXTURE_MIN_FILTER, toGL(settings.minFilter));
	}
	else {
		glSamplerParameteri(samplerID, GL_TEXTURE_MIN_FILTER, toGLNoMip(settings.minFilter));
	}

	if (settings.useLOD) {
		glSamplerParameterf(samplerID, GL_TEXTURE_MIN_LOD, settings.minLOD);
		glSamplerParameterf(samplerID, GL_TEXTURE_MAX_LOD, settings.maxLOD);
		glSamplerParameterf(samplerID, GL_TEXTURE_LOD_BIAS, settings.lodBias);
	}


	glSamplerParameteri(samplerID, GL_TEXTURE_MAG_FILTER, toGLNoMip(settings.magFilter));

	glSamplerParameteri(samplerID, GL_TEXTURE_WRAP_S, toGL(settings.wrapS));
	glSamplerParameteri(samplerID, GL_TEXTURE_WRAP_T, toGL(settings.wrapT));
	glSamplerParameteri(samplerID, GL_TEXTURE_WRAP_R, toGL(settings.wrapR));

	glSamplerParameteri(samplerID, GL_TEXTURE_COMPARE_MODE, toGL(settings.compareMode));
	glSamplerParameteri(samplerID, GL_TEXTURE_COMPARE_FUNC, toGL(settings.compareFunc));

	glSamplerParameterf(samplerID, GL_TEXTURE_MAX_ANISOTROPY_EXT, settings.maxAnisotropy);

	glSamplerParameterfv(samplerID, GL_TEXTURE_BORDER_COLOR, settings.borderColor);

	imageSizes = glm::vec2(surface->w, surface->h);

	SDL_DestroySurface(surface);
}

GlImage::GlImage(SDL_Surface *surface, ImageSettings settings)
{
	spdlog::debug("Creating image from surface memory address 0x{:x}", reinterpret_cast<uintptr_t>(surface));
	setupObject(surface, settings);
}

GlImage::GlImage(glm::vec2 size, ImageSettings settings) {
	spdlog::debug("Creating blank image {}x{}", size.x, size.y);

    SDL_Surface *blank = SDL_CreateSurface(size.x, size.y, SDL_PIXELFORMAT_RGBA32);
    if (!blank)
    {
        spdlog::critical("Blank surface failed: {}", SDL_GetError());
        std::exit(1);
    }

    SDL_ClearSurface(blank, 0.0f, 0.0f, 0.0f, 0.0f);   // fully transparent

    setupObject(blank, settings);   // converts (no-op copy), uploads, sets up sampler
    SDL_DestroySurface(blank);      // setupObject only frees its own converted copy
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

    GLubyte clearColor[4] = {0, 0, 0, 0};
    glClearTexImage(textureID, 0, GL_RGBA, GL_UNSIGNED_BYTE, clearColor);

    if (useMip) glGenerateTextureMipmap(textureID);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) spdlog::error("clearTransparent GL error: 0x{:x}", err);
}

void GlImage::updateImage(SDL_Surface *surface)
{
    if (!surface || textureID == 0) return;

    SDL_Surface *conv = surface;
    if (surface->format != SDL_PIXELFORMAT_RGBA32) {
        conv = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
        if (!conv) {
            spdlog::error("updateImage convert failed: {}", SDL_GetError());
            return;
        }
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, conv->pitch / 4);

    if (conv->w != (int)imageSizes.x || conv->h != (int)imageSizes.y) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, conv->w, conv->h, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, conv->pixels);
        imageSizes = glm::vec2(conv->w, conv->h);
    } else {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, conv->w, conv->h,
                        GL_RGBA, GL_UNSIGNED_BYTE, conv->pixels);
    }

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

    if (useMip) glGenerateMipmap(GL_TEXTURE_2D);

    if (conv != surface) SDL_DestroySurface(conv);
}

GlImage::GlImage(const char *filePath, ImageSettings settings)
{
	if (!filePath) {
		spdlog::warn("Creating image with fallback texture");
	}
	else {
		spdlog::debug("Creating image at path {}", filePath);
	}
	SDL_Surface *surf;
	loadFile(filePath, surf, &settings);
	setupObject(surf, settings);
	SDL_DestroySurface(surf);
}

GlImage::GlImage(GLuint existingTexture, glm::vec2 size)
{
	spdlog::debug("Creating image with pre existing texture id: 0x{:x} with size {},{}", existingTexture, size.x, size.y);
	if (existingTexture == 0)
	{
		spdlog::error("GlImage: trying to wrap invalid texture (0)");
		std::exit(1);
	}

	textureID = existingTexture;
	imageSizes = size;
}

extern "C" std::unique_ptr<Image> createImageFromFile(const char *filePath, ImageSettings settings) {
    return std::make_unique<GlImage>(filePath, settings);
}

extern "C" std::unique_ptr<Image> createImageFromSurface(SDL_Surface *surface, ImageSettings settings) {
    return std::make_unique<GlImage>(surface, settings);
}

extern "C" std::unique_ptr<Image> createImageFromSize(glm::vec2 size, ImageSettings settings) {
	return std::make_unique<GlImage>(size, settings);
}