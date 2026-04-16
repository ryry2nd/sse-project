#include "Rendering.hpp"

#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_pixels.h>

#include <iostream>

SDL_Color Rendering::Vec4ToSDLColor(const glm::vec4& color) {
    return SDL_Color{
        static_cast<Uint8>(color.r),
        static_cast<Uint8>(color.g),
        static_cast<Uint8>(color.b),
        static_cast<Uint8>(color.a)
    };
}

using namespace Rendering;


Uint64 Window::lastCounter = SDL_GetPerformanceCounter();
double Window::fps = 0.0f;
float Window::deltaTime = 0.0f;
Uint64 Window::now = SDL_GetTicks();
glm::vec2 Window::res;
SDL_Window *Window::window = nullptr;

void Window::Update()
{
    SDL_PumpEvents();
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    res = glm::vec2(width, height);
    now = SDL_GetTicks();
    Uint64 currentCounter = SDL_GetPerformanceCounter();
    deltaTime = static_cast<float>(currentCounter - lastCounter) / SDL_GetPerformanceFrequency();
    fps = SDL_GetPerformanceFrequency() / static_cast<double>(currentCounter - lastCounter);
    lastCounter = currentCounter;
}

const bool *Window::getKeystates(int &numKeys) {
    return SDL_GetKeyboardState(&numKeys);
}

Window::Window(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa, bool fullscreen, bool hideMouse)
{
    // it initialises sdl
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
        throw std::runtime_error("Sdl cant initialise");
    }

    if (aa > 0)
    {
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, aa);
    }

    window = SDL_CreateWindow(name, res.x, res.y, flags);
    if (!window)
    {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << "\n";
        SDL_Quit();
        throw std::runtime_error("cant create sdl window");
    }

    if (hideMouse)
        // hides the mouse
        SDL_SetWindowRelativeMouseMode(window, true);
    if (fullscreen) {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
        SDL_SetWindowResizable(window, true);
        SDL_SetWindowBordered(window, false);
    }
}

Window::~Window()
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}