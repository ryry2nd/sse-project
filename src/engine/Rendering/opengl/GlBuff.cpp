#include "GlRendering.hpp"

#include <glad/gl.h>
#include <spdlog/spdlog.h>

using namespace Engine::Rendering;
using namespace OpenGl;

GLenum GlBuff::toGLTarget(Type type) {
	switch (type) {
		case Type::Uniform:      return GL_UNIFORM_BUFFER;
		case Type::Storage:      return GL_SHADER_STORAGE_BUFFER;
	}
	return GL_SHADER_STORAGE_BUFFER;
}

GLenum GlBuff::toGLUsage(Frequency freq) {
	switch (freq) {
		case Frequency::Static:  return GL_STATIC_DRAW;
		case Frequency::Dynamic: return GL_DYNAMIC_DRAW;
		case Frequency::Stream:  return GL_STREAM_DRAW;
	}
	return GL_DYNAMIC_DRAW;
}

GlBuff::GlBuff(Type type, Frequency freq, std::size_t size, const void* data)
	: Buff(type, freq, size)
{
	spdlog::debug("Creating buffer with {} bytes", size);
	target = toGLTarget(type);
	usage  = toGLUsage(freq);

	glGenBuffers(1, &id);
	glBindBuffer(target, id);

	if (data)
	{
		glBufferData(target, size, data, usage);
	}
	else
	{
		glBufferData(target, size, nullptr, usage);
	}

	glBindBuffer(target, 0);

	spdlog::debug("Set Buffer id to: 0x{:x}", id);
}

GlBuff::~GlBuff() {
	spdlog::debug("Deleting buffer with id: 0x{:x}", id);
	glDeleteBuffers(1, &id);
}

void GlBuff::write(std::size_t offset, std::size_t size, const void* data) {
	#ifdef DEBUG
	if (offset + size > allocSize) {
		spdlog::error("Id: 0x{:x} Invalid GPU Memory space: tried to write from byte {} to byte {} but buffer ends at byte {}", id, offset, size, allocSize);
		return;
	}
	#endif

	glBindBuffer(target, id);
	glBufferSubData(target, offset, size, data);
}

void GlBuff::read(std::size_t offset, std::size_t size, void* outData) {
	#ifdef DEBUG
	if (offset + size > allocSize) {
		spdlog::error("Id: 0x{:x} Invalid GPU Memory space: tried to read from byte {} to byte {} but buffer ends at byte {}", id, offset, size, allocSize);
		return;
	}
	#endif

	glBindBuffer(target, id);
	glGetBufferSubData(target, offset, size, outData);
}