#include "GlRendering.hpp"

#include <spdlog/spdlog.h>
#include <glad/gl.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>

using namespace OpenGl;
using namespace Rendering;

GlWindow::GlWindow(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa, bool fullscreen, int vsync, bool hideMouse)
    : Window(res, name, flags | SDL_WINDOW_OPENGL, aa, fullscreen, hideMouse)
{
    // this does what it says
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // this makes the opengl content
    glContext = SDL_GL_CreateContext(window);
    if (!glContext)
    {
        spdlog::error("SDL_GL_CreateContext Error: {}", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw std::runtime_error("Had issues making the opengl context");
    }

    if (!gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress))
    {
        spdlog::error("Failed to initialize GLAD");
        SDL_GL_DestroyContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw std::runtime_error("Failed to initialize GLAD");
    }

    glViewport(0, 0, res.x, res.y);
    SDL_GL_SetSwapInterval(vsync);
    // this turns on all the cool opengl settings that probably should be the default
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glDepthFunc(GL_LESS); // or GL_LEQUAL
    glDepthMask(GL_TRUE); // enable writing to depth buffer
}

void GlWindow::clearBackground()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GlWindow::swapBuffer()
{
    SDL_GL_SwapWindow(window);
}

void GlWindow::updateScreenRes()
{
    glViewport(0, 0, res.x, res.y);
}

GlWindow::~GlWindow()
{
    SDL_GL_DestroyContext(glContext);
}