#pragma once

#include <glm/glm.hpp>
#include "Core.hpp"

namespace Engine::Rendering {
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

		static inline ImageSettings err() {
			ImageSettings s;
			s.minFilter = Filter::Nearest;
			s.magFilter = Filter::Nearest;
			s.useMip = false;

			return s;
		}
		static inline ImageSettings tex() {
			ImageSettings s;
			s.minFilter = Filter::Nearest;
			s.magFilter = Filter::Nearest;
			s.useMip = false;

			s.wrapS = WrapMode::ClampToEdge;
			s.wrapT = WrapMode::ClampToEdge;
			s.wrapR = WrapMode::ClampToEdge;

			return s;
		}
	};

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

		Surface()
		{}

		void resize(Uint32 width, Uint32 height) {
			pixels.resize(width * height);
			this->width = width;
			this->height = height;
		}

		static bool SaveBMP(const Surface *image, const char* filename);
		static void fillSurf(Surface *surf, Pixel val);
		static void loadFile(const char *filePath, Surface *surface);
	};
}