#include "Rendering.hpp"
#include <cstddef>

using namespace Rendering;

Buff::Buff(Type type, Frequency freq, std::size_t size) {
    this->buffType = type;
    this->freq = freq;
    this->allocSize = size;
}

bool Buff::canRead(Type t) {
    switch (t) {
        case Type::Uniform:      return 0;
        case Type::Storage:      return 1;
        case Type::PixelPack:    return 1;
        case Type::PixelUnpack:  return 0;
    }
    return 0;
}

bool Buff::canWrite(Type t) {
    switch (t) {
        case Type::Uniform:      return 1;
        case Type::Storage:      return 1;
        case Type::PixelPack:    return 0;
        case Type::PixelUnpack:  return 1;
    }
    return 0;
}

std::size_t Buff::getSize() {
    return allocSize;
}

Buff::Type Buff::getType() {
    return buffType;
}
Buff::Frequency Buff::getFrequency() {
    return freq;
}