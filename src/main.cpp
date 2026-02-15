#include "engine/objects/Objects.hpp"
#include "engine/rendering/base/HelperFunctions.hpp"
#include "engine/scripting/ScriptingHeaders.hpp"
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <memory>

int main(int argc, char *argv[])
{
    std::unique_ptr<Rendering::Window> renderingEngine;
    #if DEFAULT_API == 1
        Rendering::detail::initAPI("librenderingOpenGL");
        renderingEngine = Rendering::CreationFunctions::createWindow({900, 500}, "Game", SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE, 8, false, 0, true);
    #elif DEFAULT_API == 2
        throw std::runtime_error("vulkan not supported yet");
    #else
        throw std::runtime_error("unkown rendering backend");
    #endif
    // this is the constants
    const Bigint WALK_SPEED = Bigint(10);
    Bigint run_speed = Bigint("100");

    Objects::RenderObject::init();
    
    ScriptingHeaders::Package *example_package = new ScriptingHeaders::Package("packages/example");

    // starts running the game loop
    bool running = true;
    SDL_Event event;
    const Bigint *speed;

    const bool *keystates;
    int numKeys;

    const float &deltaTime = Rendering::Window::deltaTime;

    while (running)
    {
        SDL_PumpEvents();
        Rendering::Window::Update();

        // gets events
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;

            // rotates camera
            if (event.type == SDL_EVENT_MOUSE_MOTION)
            {
                Objects::globalCamera.rotateCamera({event.motion.xrel, event.motion.yrel});
            }

            // when you press escape, leave
            if (event.type == SDL_EVENT_KEY_DOWN)
            {
                SDL_Keycode key = event.key.key;
                if (key == SDLK_ESCAPE)
                {
                    running = false;
                }
                if (key == SDLK_Z)
                {
                    run_speed *= Bigint(10);
                }
                if (key == SDLK_X)
                {
                    run_speed /= Bigint(10);
                }
            }
            if (event.type == SDL_EVENT_WINDOW_RESIZED)
            {
                renderingEngine->updateScreenRes();
            }
        }

        // gets all keystates
        keystates = SDL_GetKeyboardState(&numKeys);

        // if your running, run, otherwise dont
        speed = keystates[SDL_SCANCODE_LSHIFT] ? &run_speed : &WALK_SPEED;

        // movement
        if (keystates[SDL_SCANCODE_W])
        {
            Objects::globalCamera.position -= BigVec3(Objects::globalCamera.getForwardVector() * deltaTime) * *speed;
        }
        if (keystates[SDL_SCANCODE_S])
        {
            Objects::globalCamera.position += BigVec3(Objects::globalCamera.getForwardVector() * deltaTime) * *speed;
        }

        if (keystates[SDL_SCANCODE_D])
        {
            Objects::globalCamera.position -= BigVec3(Objects::globalCamera.getRightVector() * deltaTime) * *speed;
        }
        if (keystates[SDL_SCANCODE_A])
        {
            Objects::globalCamera.position += BigVec3(Objects::globalCamera.getRightVector() * deltaTime) * *speed;
        }

        if (keystates[SDL_SCANCODE_SPACE])
        {
            Objects::globalCamera.position += BigVec3(Objects::globalCamera.getDownVector() * deltaTime) * *speed;
        }
        if (keystates[SDL_SCANCODE_LCTRL])
        {
            Objects::globalCamera.position -= BigVec3(Objects::globalCamera.getDownVector() * deltaTime) * *speed;
        }

        ScriptingHeaders::Package::LoopFunctions();

        Objects::Drawable::UpdateAllObjects();

        renderingEngine->clearBackground();

        Objects::Drawable::DrawAllObjects();

        renderingEngine->swapBuffer();
    }
    // delete everything
    delete example_package;
    return 0;
}

#ifdef _WIN32 // I HATE WINDOWS, THIS IS THE OS FOR THE DEVIL, ONLY THE DEVIL MAKES THEIR OPERATING SYSTEM THIS BAD

#include <windows.h> // EVIL EVIL EVIL, KILL ME

extern "C" int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    return main(__argc, __argv);
}
#endif