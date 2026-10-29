#include "Image.hpp"

#include <spdlog/spdlog.h>

#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace Engine::Rendering;

Surface* MakeSurfaceFromRGBA(
	const unsigned char* pixels,
	int w,
	int h)
{
	if (!pixels || w <= 0 || h <= 0)
		return nullptr;

	Surface* surface = new Surface(w, h);

	const std::size_t count =
		static_cast<std::size_t>(w) * h;

	for (std::size_t i = 0; i < count; ++i)
	{
		surface->pixels[i] = Pixel{
			pixels[i * 4 + 0], // R
			pixels[i * 4 + 1], // G
			pixels[i * 4 + 2], // B
			pixels[i * 4 + 3]  // A
		};
	}

	return surface;
}

Surface* LoadImage(const char* path)
{
	int width = 0;
	int height = 0;
	int channels = 0;

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

	Surface* surface = MakeSurfaceFromRGBA(pixels, width, height);

	stbi_image_free(pixels);

	return surface;
}

Surface* getErrorTex()
{
	const Pixel pixels[] = {
		{0,   0,   0,   255},
		{255, 0,   255, 255},
		{255, 0,   255, 255},
		{0,   0,   0,   255}
	};

	Surface* surface = new Surface(2, 2);

	std::copy(
		std::begin(pixels),
		std::end(pixels),
		surface->pixels.begin()
	);

	return surface;
}

void Image::loadFile(
	const char* filePath,
	Surface*& surface,
	ImageSettings* settings)
{
	if (!filePath)
	{
		surface = getErrorTex();
		*settings = ImageSettings::err();
		return;
	}

	const std::filesystem::path file = filePath;

	if (std::filesystem::exists(file))
	{
		surface = LoadImage(filePath);

		if (!surface || surface->pixels.empty())
		{
			spdlog::error("Failed to load image '{}'", filePath);

			delete surface;
			surface = getErrorTex();
			*settings = ImageSettings::err();
		}
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