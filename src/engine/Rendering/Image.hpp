#pragma once

#include "Core.hpp"

namespace Engine::Rendering
{
	struct ImageSettings
	{
		enum class Filter : Uint8
		{
			Nearest,
			Linear,

			NearestMipmapNearest,
			LinearMipmapNearest,
			NearestMipmapLinear,
			LinearMipmapLinear
		};

		enum class WrapMode : Uint8
		{
			Repeat,
			ClampToEdge,
			ClampToBorder,
			MirroredRepeat
		};

		enum class CompareMode : Uint8
		{
			None,
			CompareRefToTexture
		};

		enum class CompareFunc : Uint8
		{
			LessEqual,
			GreaterEqual,
			Less,
			Greater,
			Equal,
			NotEqual,
			Always,
			Never
		};

		// Filtering
		Filter minFilter = Filter::LinearMipmapLinear;
		Filter magFilter = Filter::Linear;

		// Wrapping
		WrapMode wrapS = WrapMode::Repeat;
		WrapMode wrapT = WrapMode::Repeat;
		WrapMode wrapR = WrapMode::Repeat;

		// Depth compare
		CompareMode compareMode = CompareMode::None;
		CompareFunc compareFunc = CompareFunc::LessEqual;

		// Mip
		bool useMip = true;
		int baseLevel = 0;
		int maxLevel = 9;

		// LOD
		bool useLOD = false;
		float minLOD = -1000.0f;
		float maxLOD = 1000.0f;
		float lodBias = 0.0f;

		// Border color
		float borderColor[4] = {0.f, 0.f, 0.f, 0.f};

		// Anisotropy
		float maxAnisotropy = 1.0f;
	};
	class Image
	{
	public:
		Image& operator=(const Image&) = delete;
		Image& operator=(Image&&) noexcept = default;

		virtual ~Image() = default;
		static void loadFile(const char *filePath, SDL_Surface *& surface, ImageSettings *settings);
		glm::vec2 getSizes() {return imageSizes;}
		float getWidth() {return imageSizes.x;}
		float getHeight() {return imageSizes.y;}
		virtual void clearTransparent() = 0;
	protected:
		glm::vec2 imageSizes;
	};
}