#include "Rendering.hpp"

using namespace Rendering;

Buff::Buff(Type type, Frequency freq, std::size_t size) {
    this->buffType = type;
    this->freq = freq;
    this->allocSize = size;
}