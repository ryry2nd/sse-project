#include "Rendering.hpp"

using namespace Rendering;

glm::vec2 FrameBuffer::getSize() const {
	return size;
}

uint32_t FrameBuffer::getSettings() const {
	return settings;
}