#include "engine/objects/Objects.hpp"
#include "engine/HelperFunctions.hpp"
#include "engine/scripting/ScriptingHeaders.hpp"
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <iostream>

#ifdef USE_OPENGL
#include "API/opengl/HelperFunctionsOpengl.hpp"
#endif

int main(int argc, char *argv[])
{
    Rendering::HelperFunctions *renderingEngine;
    #if DEFAULT_API == 1
        renderingEngine = new OpenGl::HelperFunctionsApi({900, 500}, "Game", SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE, 8, false, 0, true);
        Rendering::init(new OpenGl::MeshApi(), new OpenGl::ShaderApi(), new OpenGl::ImageApi());
    #elif DEFAULT_API == 2
        throw std::runtime_error("vulkan not supported yet");
    #elif DEFAULT_API == 3
        throw std::runtime_error("opengl es not supported yet");
    #else
        throw std::runtime_error("idk what you did but this thing compleatly broke");
    #endif
    // this is the constants
    const Bigint WALK_SPEED = Bigint(10);
    Bigint run_speed = Bigint("100");

    Objects::RenderObject::init();
    ScriptingHeaders::GameLibrary gameLib("game");

    // starts running the game loop
    bool running = true;
    SDL_Event event;
    const Bigint *speed;

    const bool *keystates;
    int numKeys;

    const float &deltaTime = Rendering::HelperFunctions::deltaTime;

    while (running)
    {
        SDL_PumpEvents();
        Rendering::HelperFunctions::Update();

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

        Objects::Drawable::UpdateAllObjects();

        renderingEngine->clearBackground();

        Objects::Drawable::DrawAllObjects();

        //         ss.str("");
        //         ss.clear();
        //         ss << std::fixed << std::setprecision(0) << fps;
        //
        //         text = Rendering::defaultImageAPI->makeNewImage(gameLib.fonts[0]->renderText(ss.str(), {255, 255, 255, 255}));
        //         speedometer = Rendering::defaultImageAPI->makeNewImage(gameLib.fonts[0]->renderText(run_speed.toString(), {255, 255, 255, 255}));
        //
        //         object2d.position.x = (text->imageSizes.x * object2d.scale.x) / 2;
        //         object2d.Draw(text);
        //         speed2d.position.x = (speedometer->imageSizes.x * speed2d.scale.x) / 2;
        //         speed2d.Draw(speedometer);

        renderingEngine->swapBuffer();

        // delete text;
        // delete speedometer;
    }
    // delete everything
    delete renderingEngine;
    return 0;
}

#ifdef _WIN32 // I HATE WINDOWS, THIS IS THE OS FOR THE DEVIL, ONLY THE DEVIL MAKES THEIR OPERATING SYSTEM THIS BAD

#include <windows.h> // EVIL EVIL EVIL, KILL ME

extern "C" int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    return main(__argc, __argv);
}
#endif