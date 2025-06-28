#include "HelperFunctionsOpengl.hpp"

HelperFunctionsOpenGl::HelperFunctionsOpenGl(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa, bool fullscreen, int vsync, bool hideMouse)
    : HelperFunctions(res, name, flags, aa, fullscreen, hideMouse)
{
    // this does what it says
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // this makes the opengl content
    glContext = SDL_GL_CreateContext(window);
    if (!glContext)
    {
        std::cerr << "SDL_GL_CreateContext Error: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw std::runtime_error("Had issues making the opengl context");
    }

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW\n";
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw std::runtime_error("Failed to initialize GLEW\n");
    }

    glViewport(0, 0, res.x, res.y);

    SDL_GL_SetSwapInterval(vsync);

    // this turns on all the cool opengl settings that probably should be the default
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
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

void HelperFunctionsOpenGl::clearBackground()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void HelperFunctionsOpenGl::swapBuffer()
{
    SDL_GL_SwapWindow(window);
}

HelperFunctionsOpenGl::~HelperFunctionsOpenGl()
{
    SDL_GL_DeleteContext(glContext);
}