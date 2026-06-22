#include "Image.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_surface.h>
#include <spdlog/spdlog.h>
#include <cstring>
#include <filesystem>
#include <png.h>
#include <jpeglib.h>
#include <cstdio>

using namespace Engine::Rendering;

SDL_Surface* MakeSurfaceFromRGBA(unsigned char* pixels, int w, int h)
{
	SDL_Surface* surface = SDL_CreateSurface(
		w,
		h,
		SDL_PIXELFORMAT_RGBA32
	);

	if (!surface) return nullptr;

	std::memcpy(surface->pixels, pixels, w * h * 4);
	return surface;
}

SDL_Surface* LoadPNG(const char* path)
{
	FILE* fp = fopen(path, "rb");
	if (!fp) return nullptr;

	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	png_infop info = png_create_info_struct(png);

	png_init_io(png, fp);
	png_read_info(png, info);

	int width = png_get_image_width(png, info);
	int height = png_get_image_height(png, info);

	png_set_expand(png);
	png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
	png_read_update_info(png, info);

	unsigned char* pixels = new unsigned char[width * height * 4];

	png_bytep rows[height];
	for (int y = 0; y < height; y++)
		rows[y] = pixels + y * width * 4;

	png_read_image(png, rows);

	png_destroy_read_struct(&png, &info, nullptr);
	fclose(fp);

	SDL_Surface* surf = MakeSurfaceFromRGBA(pixels, width, height);

	delete[] pixels; // SDL copy already done
	return surf;
}

SDL_Surface* LoadJPG(const char* path)
{
	FILE* fp = fopen(path, "rb");
	if (!fp) return nullptr;

	jpeg_decompress_struct cinfo;
	jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	jpeg_stdio_src(&cinfo, fp);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);

	int width = cinfo.output_width;
	int height = cinfo.output_height;
	int channels = cinfo.output_components;

	unsigned char* pixels = new unsigned char[width * height * channels];

	while (cinfo.output_scanline < height)
	{
		unsigned char* row = pixels + cinfo.output_scanline * width * channels;
		jpeg_read_scanlines(&cinfo, &row, 1);
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	fclose(fp);

	SDL_Surface* surf;

	if (channels == 3)
	{
		surf = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_RGB24);
		std::memcpy(surf->pixels, pixels, width * height * 3);
	}
	else
	{
		surf = MakeSurfaceFromRGBA(pixels, width, height);
	}

	delete[] pixels;
	return surf;
}

SDL_Surface *Image::loadFile(const char *filePath)
{
	std::filesystem::path file = filePath;

	SDL_Surface *surface = nullptr;

	if (file.extension() == ".jpg" || file.extension() == ".jpeg") surface = LoadJPG(filePath);
	else if (file.extension() == ".png") surface = LoadPNG(filePath);
	else {
		spdlog::error("Image extention: {} not known", file.extension().string());
		std::exit(1);
	}

	if (!surface)
	{
		spdlog::error("Image load fail: {}", SDL_GetError());
		std::exit(1);
	}
	return surface;
}

SDL_Surface *Image::getErrorTex() {
	unsigned char purple[4] = {255, 0, 255, 255};
	unsigned char black[4]  = {0, 0, 0, 255};

	unsigned char pixels[4*4];

	memcpy(pixels + 0, black, 4);
	memcpy(pixels + 4, purple, 4);
	memcpy(pixels + 8, purple, 4);
	memcpy(pixels + 12, black, 4);

	SDL_Surface *surface = MakeSurfaceFromRGBA(&pixels[0], 2, 2);

	return surface;
}