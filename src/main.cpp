#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "engine/objects/Objects.hpp"
#include "engine/HelperFunctions.hpp"
#include "engine/scripting/ScriptingHeaders.hpp"
#include "engine/opengl/HelperFunctionsOpengl.hpp"
#include <string>
#include <memory>
#include <iostream>

int main(int argc, char *argv[])
{
    glm::vec2 res(900, 500);
    Rendering::HelperFunctions *renderingEngine = new OpenGl::HelperFunctionsApi(res, "Game", SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE, 8, false, 0, true);
    Rendering::init(new OpenGl::MeshApi(), new OpenGl::ShaderApi(), new OpenGl::ImageApi());
    GameLibrary gameLib("game");
    // this is the constants
    const float MOUSE_SENSITIVITY = 0.1;
    const Bigint WALK_SPEED = Bigint(10);
    Bigint run_speed = Bigint("100");

    // this is the camera, cameras are neat
    Objects::Camera *camera = new Objects::Camera(res, BigVec3(0.0f, 0.0f, -2.0f));
    camera->rotation.y = 180.0f;

    Objects::RenderObject::init(gameLib.shaders["point_shader"], camera);

    Objects::RenderObject2d object2d(gameLib.shaders["2d_shader"], camera);
    object2d.scale = glm::vec2(0.5f);
    object2d.position.x = 20;
    object2d.position.y = res.y - 24;

    Objects::RenderObject2d speed2d(gameLib.shaders["2d_shader"], camera);
    speed2d.scale = glm::vec2(0.5f);
    speed2d.position.x = 24;
    speed2d.position.y = res.y - 72;

    LuaHeaders::LuaScriptLib lua;
    lua.include(LuaHeaders::LuaLibEnum::glm);
    lua.include(LuaHeaders::LuaLibEnum::bigvars);
    lua.include(LuaHeaders::LuaLibEnum::objects);

    lua.includeInitialized("shader1", *gameLib.shaders["basic_shader"]);
    lua.includeInitialized("image1", *gameLib.images["Fish"]);
    lua.includeInitialized("camera1", *camera);

    lua.includeScripts("./game/scripts");

    // lua.getFunction("test")();

    // starts running the game loop
    bool running = true;
    Uint64 lastCounter = SDL_GetPerformanceCounter();
    Uint64 currentCounter;
    SDL_Event event;
    float deltaTime;
    double fps;
    const Bigint *speed;

    int accumulatedMouseX = 0;
    int accumulatedMouseY = 0;

    bool shouldUpdate;
    Uint64 prevFrameUpdate = SDL_GetPerformanceCounter();

    std::ostringstream ss;
    Rendering::Image *text;
    Rendering::Image *speedometer;

    while (running)
    {
        currentCounter = SDL_GetPerformanceCounter();
        deltaTime = static_cast<float>(currentCounter - lastCounter) / SDL_GetPerformanceFrequency();
        fps = SDL_GetPerformanceFrequency() / static_cast<double>(currentCounter - lastCounter);
        lastCounter = currentCounter;

        // gets events
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = false;

            // rotates camera
            if (event.type == SDL_MOUSEMOTION)
            {
                accumulatedMouseX += event.motion.xrel;
                accumulatedMouseY += event.motion.yrel;
            }

            // when you press escape, leave
            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    running = false;
                }
                if (event.key.keysym.sym == SDLK_z)
                {
                    run_speed *= Bigint(10);
                }
                if (event.key.keysym.sym == SDLK_x)
                {
                    run_speed /= Bigint(10);
                }
            }
        }

        // gets all keystates
        const Uint8 *keystates = SDL_GetKeyboardState(NULL);

        // if your running, run, otherwise dont
        speed = keystates[SDL_SCANCODE_LSHIFT] ? &run_speed : &WALK_SPEED;

        // movement
        if (keystates[SDL_SCANCODE_W])
        {
            camera->position -= BigVec3(camera->getForwardVector() * deltaTime) * *speed;
        }
        if (keystates[SDL_SCANCODE_S])
        {
            camera->position += BigVec3(camera->getForwardVector() * deltaTime) * *speed;
        }

        if (keystates[SDL_SCANCODE_D])
        {
            camera->position -= BigVec3(camera->getRightVector() * deltaTime) * *speed;
        }
        if (keystates[SDL_SCANCODE_A])
        {
            camera->position += BigVec3(camera->getRightVector() * deltaTime) * *speed;
        }

        if (keystates[SDL_SCANCODE_SPACE])
        {
            camera->position += BigVec3(camera->getDownVector() * deltaTime) * *speed;
        }
        if (keystates[SDL_SCANCODE_LCTRL])
        {
            camera->position -= BigVec3(camera->getDownVector() * deltaTime) * *speed;
        }

        if (deltaTime > 0)
        {
            camera->rotation.y -= (accumulatedMouseX / deltaTime) * MOUSE_SENSITIVITY * deltaTime;
            camera->rotation.x -= (accumulatedMouseY / deltaTime) * MOUSE_SENSITIVITY * deltaTime;

            accumulatedMouseX = 0;
            accumulatedMouseY = 0;
        }

        Objects::RenderObject::updateTime();

        Objects::RenderObject::UpdateAllObjects(deltaTime);

        renderingEngine->clearBackground();

        Objects::RenderObject::DrawAllObjects();

        ss.str("");
        ss.clear();
        ss << std::fixed << std::setprecision(0) << fps;

        text = Rendering::defaultImageAPI->makeNewImage(gameLib.fonts["default_font"]->renderText(ss.str(), {255, 255, 255, 255}));
        speedometer = Rendering::defaultImageAPI->makeNewImage(gameLib.fonts["default_font"]->renderText(run_speed.toString(), {255, 255, 255, 255}));

        object2d.position.x = (text->imageSizes.x * object2d.scale.x) / 2;
        object2d.Draw(text);
        speed2d.position.x = (speedometer->imageSizes.x * speed2d.scale.x) / 2;
        speed2d.Draw(speedometer);

        renderingEngine->swapBuffer();

        delete text;
        delete speedometer;
    }
    // delete everything
    delete renderingEngine;
    delete camera;
    return 0;
}
