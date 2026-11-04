#include "Rendering.hpp"
#include "glad/gl.h"

GLenum toGLTarget(Buff::Type type) {
	switch (type) {
		case Buff::Type::Uniform:      return GL_UNIFORM_BUFFER;
		case Buff::Type::Storage:      return GL_SHADER_STORAGE_BUFFER;
	}
	return GL_SHADER_STORAGE_BUFFER;
}

GLenum toGLUsage(Buff::Frequency freq) {
	switch (freq) {
		case Buff::Frequency::Static:  return GL_STATIC_DRAW;
		case Buff::Frequency::Dynamic: return GL_DYNAMIC_DRAW;
		case Buff::Frequency::Stream:  return GL_STREAM_DRAW;
	}
	return GL_DYNAMIC_DRAW;
}

APIBuff::APIBuff(Type type, Frequency freq, std::size_t size, const void* data)
{
	SCOPE("Rendering::OpenGL::Buff::create");
	this->allocSize = size;

	target = toGLTarget(type);
	GLenum usage  = toGLUsage(freq);

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
}

APIBuff::APIBuff(GLuint existingID, Type type, std::size_t size)
{
    id = existingID;
    allocSize = size;
    target = toGLTarget(type);
}

APIBuff::~APIBuff() {
	SCOPE("Rendering::OpenGL::Buff::destroy");
	glDeleteBuffers(1, &id);
}

void APIBuff::write(std::size_t offset, std::size_t size, const void* data) {
	SCOPE("Rendering::OpenGL::Buff::write");
	#ifdef DEBUG
	if (offset > allocSize || offset + size > allocSize) {
		spdlog::critical("Id: 0x{:x} Invalid GPU Memory space: tried to write from byte {} to byte {} but buffer ends at byte {}\nThis Error does not occur off of debug. Fix as soon as possible otherwise memory issues will occur.", id, offset, size, allocSize);
		std::exit(1);
	}
	#endif

	glBindBuffer(target, id);
	glBufferSubData(target, offset, size, data);
}

void APIBuff::read(std::size_t offset, std::size_t size, void* outData) {
	SCOPE("Rendering::OpenGL::Buff::read");
	#ifdef DEBUG
	if (offset > allocSize || offset + size > allocSize) {
		spdlog::critical("Id: 0x{:x} Invalid GPU Memory space: tried to read from byte {} to byte {} but buffer ends at byte {}\nThis Error does not occur off of debug. Fix as soon as possible otherwise memory issues will occur.", id, offset, size, allocSize);
		std::exit(1);
	}
	#endif

	glBindBuffer(target, id);
	glGetBufferSubData(target, offset, size, outData);
}

void APIBuff::copy(
    const Buff *src,
    std::size_t sourceOffset,
    std::size_t destinationOffset,
    std::size_t size)
{
    SCOPE("Rendering::OpenGL::Buff::copy");

	const APIBuff *source = static_cast<const APIBuff *>(src);

    #ifdef DEBUG
    if (sourceOffset > source->allocSize ||
        size > source->allocSize - sourceOffset)
    {
        spdlog::critical(
            "Source buffer copy out of bounds. ID: 0x{:x}",
            source->id
        );
        std::exit(1);
    }

    if (destinationOffset > allocSize ||
        size > allocSize - destinationOffset)
    {
        spdlog::critical(
            "Destination buffer copy out of bounds. ID: 0x{:x}",
            id
        );
        std::exit(1);
    }
    #endif

    glBindBuffer(GL_COPY_READ_BUFFER, source->id);
    glBindBuffer(GL_COPY_WRITE_BUFFER, id);

    glCopyBufferSubData(
        GL_COPY_READ_BUFFER,
        GL_COPY_WRITE_BUFFER,
        sourceOffset,
        destinationOffset,
        size
    );

    glBindBuffer(GL_COPY_READ_BUFFER, 0);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
}

extern "C" std::unique_ptr<Buff> createBuff(Buff::Type type, Buff::Frequency freq, std::size_t size, const void* data = nullptr) {
    return std::make_unique<APIBuff>(type, freq, size, data);
}
