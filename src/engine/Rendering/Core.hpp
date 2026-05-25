#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

struct SDL_Surface;
struct SDL_Window;
struct SDL_Color;

typedef uint32_t Uint32;
typedef uint64_t Uint64;
typedef uint8_t Uint8;

namespace Engine::Rendering
{
	SDL_Color Vec4ToSDLColor(const glm::vec4& color);
}