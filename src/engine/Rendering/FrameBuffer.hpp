#pragma once

#include "Image.hpp"
#include "RenderTarget.hpp"

namespace Engine::Rendering
{
	class FrameBuffer : public RenderTarget {
	public:
		enum Settings : uint32_t {
			Color = 1 << 0,
			Depth = 1 << 1,
			Stencil = 1 << 2,
		};

		virtual ~FrameBuffer() = default;
		uint32_t getSettings() {return settings;}
		virtual void setSize(glm::vec2 size) = 0;

		virtual void setBackgroundColor(glm::vec4 color) = 0;
		virtual void clearBackground() = 0;

		virtual Image* getColorImage() = 0;
		virtual Image* getDepthImage() = 0;
		virtual Image* getStencilImage() = 0;

	protected:
		uint32_t settings;
	};
}