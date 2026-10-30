#pragma once

#include "Core.hpp"
#include "Surface.hpp"

namespace Engine::Rendering
{

	class Image
	{
	public:
		Image& operator=(const Image&) = delete;
		Image& operator=(Image&&) noexcept = default;

		virtual ~Image() = default;
		glm::vec2 getSizes() {return imageSizes;}
		float getWidth() {return imageSizes.x;}
		float getHeight() {return imageSizes.y;}
		virtual void clearTransparent() = 0;
		virtual void updateImage(const Surface *surface) = 0;
	protected:
		glm::vec2 imageSizes;
	};
}