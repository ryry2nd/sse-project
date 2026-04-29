#include "GlRendering.hpp"

#include <spdlog/spdlog.h>
#include <glad/gl.h>

using namespace Rendering;
using namespace OpenGl;

GLenum GlBuff::toGLTarget(Type type) {
    switch (type) {
        case Type::Uniform:      return GL_UNIFORM_BUFFER;
        case Type::Storage:      return GL_SHADER_STORAGE_BUFFER;
        // case Type::PixelPack:    return GL_PIXEL_PACK_BUFFER;
        // case Type::PixelUnpack:  return GL_PIXEL_UNPACK_BUFFER;
    }
    return GL_ARRAY_BUFFER;
}

GLenum GlBuff::toGLUsage(Frequency freq) {
    switch (freq) {
        case Frequency::Static:  return GL_STATIC_DRAW;
        case Frequency::Dynamic: return GL_DYNAMIC_DRAW;
        case Frequency::Stream:  return GL_STREAM_DRAW;
    }
    return GL_STATIC_DRAW;
}

GlBuff::GlBuff(Type type, Frequency freq, std::size_t size, const void* data)
    : Buff(type, freq, size)
{
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
}

GlBuff::~GlBuff() {
    glDeleteBuffers(1, &id);
}

void GlBuff::write(std::size_t offset, std::size_t size, const void* data) {
    #ifdef DEBUG
    if (offset + size > allocSize) {
        spdlog::error("Invalid GPU Memory space: tried to write from byte {} to byte {} but buffer ends at byte {}", offset, size, allocSize);
        return;
    }
    if (!canWrite(buffType)) {
        spdlog::error("Tried to write to buffer type that is not allowed to be written to");
        return;
    }
    #endif

    glBindBuffer(target, id);
    glBufferSubData(target, offset, size, data);
}

void GlBuff::read(std::size_t offset, std::size_t size, void* outData) {
    #ifdef DEBUG
    if (offset + size > allocSize) {
        spdlog::error("Invalid GPU Memory space: tried to read from byte {} to byte {} but buffer ends at byte {}", offset, size, allocSize);
        return;
    }
    if (!canRead(buffType)) {
        spdlog::error("Tried to read to buffer type that is not allowed to be written to");
        return;
    }
    #endif

    glBindBuffer(target, id);
    glGetBufferSubData(target, offset, size, outData);
}

GLuint GlBuff::getID() {
    return id;
}

GLenum GlBuff::getTarget() {
    return target;
}
GLenum GlBuff::getUsage() {
    return usage;
}