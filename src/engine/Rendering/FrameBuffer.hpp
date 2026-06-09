#pragma once

#include "Image.hpp"

namespace Engine::Rendering
{
	class FrameBuffer {
	public:
		enum Settings : uint32_t {
			Color = 1 << 0,
			Depth = 1 << 1,
			Stencil = 1 << 2,
		};

		virtual ~FrameBuffer() = default;
		glm::vec2 getRes() {return res;}
		float getWidth() {return res.x;}
		float getHeight() {return res.y;}
		uint32_t getSettings() {return settings;}
		virtual void setSize(glm::vec2 size) = 0;

		virtual Image* getColorImage() = 0;
		virtual Image* getDepthImage() = 0;
		virtual Image* getStencilImage() = 0;

	protected:
		glm::vec2 res;
		uint32_t settings;
	};
}