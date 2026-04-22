#include "GlRendering.hpp"

#include <glad/gl.h>

using namespace Rendering;
using namespace OpenGl;

GLenum GlBuff::toGLTarget(Type type) {
    switch (type) {
        case Type::Array:        return GL_ARRAY_BUFFER;
        case Type::Element:      return GL_ELEMENT_ARRAY_BUFFER;
        case Type::Uniform:      return GL_UNIFORM_BUFFER;
        case Type::Storage:      return GL_SHADER_STORAGE_BUFFER;
        case Type::PixelPack:    return GL_PIXEL_PACK_BUFFER;
        case Type::PixelUnpack:  return GL_PIXEL_UNPACK_BUFFER;
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

GlBuff::GlBuff(Type type, Frequency freq, std::size_t size, void* data)
    : Buff(type, freq, size)
{
    target = toGLTarget(type);
    usage  = toGLUsage(freq);

    glGenBuffers(1, &id);
    glBindBuffer(target, id);

    glBufferData(target, size, data, usage); // allocate
}

GlBuff::~GlBuff() {
    glDeleteBuffers(1, &id);
}

void GlBuff::write(std::size_t offset, std::size_t size, const void* data) {
    assert(offset + size <= allocSize);
    assert(canWrite(buffType));

    glBindBuffer(target, id);
    glBufferSubData(target, offset, size, data);
}

void GlBuff::read(std::size_t offset, std::size_t size, void* outData) {
    assert(offset + size <= allocSize);
    assert(canRead(buffType));

    glBindBuffer(target, id);
    glGetBufferSubData(target, offset, size, outData);
}