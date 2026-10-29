#include "Font.hpp"
#include "Surface.hpp"

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <string>

#include "apis/internal/Scope.hpp"

using namespace Engine::Rendering;

Font::Font(const char* path, size_t size)
	: fontSize(size)
{
	SCOPE("Engine::Rendering::Font::create");

	if (!path || size == 0)
		return;

	std::ifstream file(path, std::ios::binary);

	if (!file)
		return;

	fontData.assign(
		std::istreambuf_iterator<char>(file),
		std::istreambuf_iterator<char>()
	);

	if (fontData.empty())
		return;

	auto* info = new stbtt_fontinfo{};

	if (!stbtt_InitFont(
			info,
			fontData.data(),
			stbtt_GetFontOffsetForIndex(fontData.data(), 0)))
	{
		delete info;
		fontData.clear();
		return;
	}

	fontInfo = info;
	scale = stbtt_ScaleForPixelHeight(
		info, static_cast<float>(size));
}

Font::~Font()
{
	SCOPE("Engine::Rendering::Font::destroy");
	delete static_cast<stbtt_fontinfo*>(fontInfo);
}

Surface* Font::RenderText(const char* text, glm::vec4 color)
{
	SCOPE("Engine::Rendering::Font::renderText");

	auto* info = static_cast<stbtt_fontinfo*>(fontInfo);

	if (!info || !text || !*text)
		return nullptr;

	const std::string str(text);

	int ascent = 0, descent = 0, lineGap = 0;
	stbtt_GetFontVMetrics(info, &ascent, &descent, &lineGap);

	const int baseline = static_cast<int>(std::ceil(ascent * scale));
	const int height = static_cast<int>(
		std::ceil((ascent - descent) * scale));

	if (height <= 0)
		return nullptr;

	float penX = 0.0f;
	int previousGlyph = 0;
	int minX = 0;
	int maxX = 0;

	// Measure the text.
	for (unsigned char c : str)
	{
		if (previousGlyph != 0)
		{
			penX += stbtt_GetCodepointKernAdvance(
				info, previousGlyph, c) * scale;
		}

		int advance = 0, bearing = 0;
		stbtt_GetCodepointHMetrics(info, c, &advance, &bearing);

		int x0, y0, x1, y1;
		stbtt_GetCodepointBitmapBox(
			info, c, scale, scale, &x0, &y0, &x1, &y1);

		minX = std::min(
			minX, static_cast<int>(std::floor(penX)) + x0);
		maxX = std::max(
			maxX, static_cast<int>(std::floor(penX)) + x1);

		penX += advance * scale;
		previousGlyph = c;
	}

	maxX = std::max(maxX, static_cast<int>(std::ceil(penX)));

	const int width = maxX - minX;

	if (width <= 0)
		return nullptr;

	const auto red = static_cast<std::uint8_t>(
		std::round(std::clamp(color.r, 0.0f, 1.0f) * 255.0f));
	const auto green = static_cast<std::uint8_t>(
		std::round(std::clamp(color.g, 0.0f, 1.0f) * 255.0f));
	const auto blue = static_cast<std::uint8_t>(
		std::round(std::clamp(color.b, 0.0f, 1.0f) * 255.0f));
	const float alpha = std::clamp(color.a, 0.0f, 1.0f);

	// Surface pixels are initialized to transparent black.
	Surface* surface = new Surface(width, height);

	// Rasterize glyphs directly into the surface.
	penX = 0.0f;
	previousGlyph = 0;

	for (unsigned char c : str)
	{
		if (previousGlyph != 0)
		{
			penX += stbtt_GetCodepointKernAdvance(
				info, previousGlyph, c) * scale;
		}

		int advance = 0, bearing = 0;
		stbtt_GetCodepointHMetrics(info, c, &advance, &bearing);

		int x0 = 0, y0 = 0, x1 = 0, y1 = 0;

		stbtt_GetCodepointBitmapBox(
			info, c, scale, scale, &x0, &y0, &x1, &y1);

		const int glyphX =
			static_cast<int>(std::floor(penX)) + x0 - minX;
		const int glyphY = baseline + y0;

		const int glyphWidth = x1 - x0;
		const int glyphHeight = y1 - y0;

		if (glyphWidth > 0 && glyphHeight > 0)
		{
			unsigned char* bitmap = stbtt_GetCodepointBitmap(
				info, scale, scale, c,
				nullptr, nullptr, nullptr, nullptr);

			if (bitmap)
			{
				for (int row = 0; row < glyphHeight; ++row)
				{
					for (int col = 0; col < glyphWidth; ++col)
					{
						const int dx = glyphX + col;
						const int dy = glyphY + row;

						if (dx < 0 || dx >= width ||
							dy < 0 || dy >= height)
							continue;

						const unsigned char coverage =
							bitmap[
								static_cast<std::size_t>(row) *
								glyphWidth + col
							];

						if (coverage == 0)
							continue;

						Pixel& pixel = surface->pixels[
							static_cast<std::size_t>(dy) * width + dx
						];

						pixel.r = red;
						pixel.g = green;
						pixel.b = blue;
						pixel.a = static_cast<std::uint8_t>(
							std::round(coverage * alpha));
					}
				}

				stbtt_FreeBitmap(bitmap, nullptr);
			}
		}

		penX += advance * scale;
		previousGlyph = c;
	}

	return surface;
}