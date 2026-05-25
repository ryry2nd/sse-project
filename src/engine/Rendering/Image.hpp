#pragma once

#include "Core.hpp"

namespace Engine::Rendering
{
	class Image
	{
	public:
		Image& operator=(const Image&) = delete;
		Image& operator=(Image&&) noexcept = default;

		virtual ~Image() = default;
		static SDL_Surface *loadFile(const char *filePath);
		glm::vec2 getSizes() {return imageSizes;}
		virtual void clearTransparent() = 0;
	protected:
		glm::vec2 imageSizes;
	};
}