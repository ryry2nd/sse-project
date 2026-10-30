#include "Surface.hpp"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <limits>
#include <vector>

using namespace Engine::Rendering;

void Surface::fillSurf(Surface* surf, Pixel val)
{
	if (!surf)
		return;

	std::fill(
		surf->pixels.begin(),
		surf->pixels.end(),
		val
	);
}

bool Surface::SaveBMP(const Surface* image, const char* filename)
{
	if (!image || !filename)
		return false;

	if (image->pixels.size() !=
		static_cast<std::size_t>(image->width) * image->height)
		return false;

	// BMP uses 4-byte-aligned rows.
	const std::uint64_t rowSize =
		(static_cast<std::uint64_t>(image->width) * 3 + 3) & ~std::uint64_t(3);

	const std::uint64_t pixelDataSize = rowSize * image->height;
	const std::uint64_t fileSize = 54 + pixelDataSize;

	// BMP file sizes and dimensions are stored as 32-bit values.
	if (fileSize > std::numeric_limits<std::uint32_t>::max() ||
		image->width > static_cast<Uint32>(
			std::numeric_limits<std::int32_t>::max()) ||
		image->height > static_cast<Uint32>(
			std::numeric_limits<std::int32_t>::max()))
		return false;

	std::ofstream file(filename, std::ios::binary);
	if (!file)
		return false;

	auto write16 = [&](std::uint16_t v) {
		file.put(static_cast<char>(v));
		file.put(static_cast<char>(v >> 8));
	};

	auto write32 = [&](std::uint32_t v) {
		write16(static_cast<std::uint16_t>(v));
		write16(static_cast<std::uint16_t>(v >> 16));
	};

	// BMP file header (14 bytes)
	file.put('B');
	file.put('M');
	write32(static_cast<std::uint32_t>(fileSize));
	write16(0);
	write16(0);
	write32(54);

	// BITMAPINFOHEADER (40 bytes)
	write32(40);
	write32(image->width);
	write32(image->height); // Positive height = bottom-up rows
	write16(1);             // Planes
	write16(24);            // Bits per pixel
	write32(0);             // BI_RGB (no compression)
	write32(static_cast<std::uint32_t>(pixelDataSize));
	write32(2835);          // Horizontal resolution
	write32(2835);          // Vertical resolution
	write32(0);
	write32(0);

	// Pixel data: BMP stores BGR; row padding must be zeroed.
	std::vector<std::uint8_t> row(
		static_cast<std::size_t>(rowSize), 0);

	for (std::uint32_t y = image->height; y > 0; --y)
	{
		const std::size_t rowStart =
			static_cast<std::size_t>(y - 1) * image->width;

		for (Uint32 x = 0; x < image->width; ++x)
		{
			const Pixel& pixel = image->pixels[rowStart + x];
			const std::size_t dst = static_cast<std::size_t>(x) * 3;

			row[dst + 0] = pixel.b;
			row[dst + 1] = pixel.g;
			row[dst + 2] = pixel.r;
		}

		file.write(
			reinterpret_cast<const char*>(row.data()),
			static_cast<std::streamsize>(row.size())
		);

		if (!file)
			return false;
	}

	return file.good();
}