
#pragma once

#include "Core.hpp"
#include "Surface.hpp"

namespace Engine::Rendering {

class Font {
public:
	Font(const char* path, size_t size);
	~Font();

	Font(const Font&) = delete;
	Font& operator=(const Font&) = delete;

	Surface *RenderText(const char* text, glm::vec4 color);

private:
	std::vector<unsigned char> fontData;
	void* fontInfo = nullptr;
	float scale = 0.0f;
	size_t fontSize = 0;
};

}
