
#pragma once

#include "Core.hpp"

namespace Engine::Rendering {

class Font {
public:
    Font(const char* path, size_t size);
    ~Font();

    Font(const Font&) = delete;
    Font& operator=(const Font&) = delete;

    SDL_Surface* RenderText(const char* text, glm::vec4 color);
	static void deleteSurf(SDL_Surface*);

private:
    std::vector<unsigned char> fontData;
    void* fontInfo = nullptr;
    float scale = 0.0f;
    size_t fontSize = 0;
};

}
