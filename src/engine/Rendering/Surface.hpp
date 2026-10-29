#pragma once

#include <glm/glm.hpp>
#include "Core.hpp"

namespace Engine::Rendering {
	struct Pixel {
		Uint8 r;
		Uint8 g;
		Uint8 b;
		Uint8 a;
	};
	struct Surface {
		Uint32 width;
		Uint32 height;
		std::vector<Pixel> pixels;

		Surface(std::uint32_t width, std::uint32_t height)
			: width(width),
			height(height),
			pixels(static_cast<std::size_t>(width) * height, Pixel{0, 0, 0, 0})
		{}

		static bool SaveBMP(const Surface *image, const char* filename);
		static void fillSurf(Surface *surf, Pixel val);
		static void deleteSurf(Surface *);
	};
}