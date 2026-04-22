#include "GlRendering.hpp"

using namespace Rendering;
using namespace OpenGl;

GlBuff::GlBuff(Type type, Frequency freq, std::size_t size) : Buff(type, freq, size) {}
GlBuff::~GlBuff() {}
void GlBuff::write(const std::size_t offset, const std::size_t size, const void* data) {}
void GlBuff::read(const std::size_t offset, const std::size_t size, void* outData) {}