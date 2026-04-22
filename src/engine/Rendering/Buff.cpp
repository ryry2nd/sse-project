#include "Rendering.hpp"

using namespace Rendering;

Buff::Buff(Type type, Frequency freq, std::size_t size) {
    this->buffType = type;
    this->freq = freq;
    this->allocSize = size;
}

bool Buff::canRead(Type t) {
    switch (t) {
        case Type::Array:        return 1;
        case Type::Element:      return 1;
        case Type::Uniform:      return 0;
        case Type::Storage:      return 1;
        case Type::PixelPack:    return 1;
        case Type::PixelUnpack:  return 0;
    }
    return 0;
}

bool Buff::canWrite(Type t) {
    switch (t) {
        case Type::Array:        return 1;
        case Type::Element:      return 1;
        case Type::Uniform:      return 1;
        case Type::Storage:      return 1;
        case Type::PixelPack:    return 0;
        case Type::PixelUnpack:  return 1;
    }
    return 0;
}