#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "engine/objects/Objects.hpp"
#include "engine/HelperFunctions.hpp"
#include "engine/scripting/ScriptingHeaders.hpp"
#include <string>
#include <memory>
#include <iostream>

std::vector<float> vertices = {
    //  x,     y,    u,   v
    -0.5f, -0.5f, 0.0f, 1.0f, // bottom-left
    0.5f, -0.5f, 1.0f, 1.0f,  // bottom-right
    0.5f, 0.5f, 1.0f, 0.0f,   // top-right
    -0.5f, 0.5f, 0.0f, 0.0f   // top-left
};

std::vector<unsigned int> indices = {
    0, 1, 2,
    2, 3, 0};

int Bigint::numCopies = 0;
int Bigint::numMoves = 0;

int main(int argc, char *argv[])
{
    glm::vec2 res(900, 500);
    HelperFunctions *renderingEngine = new HelperFunctionsOpenGl(res, "Game", SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE, 8, false, 0, true);

    Shader *apiShader = new ShaderOpenGl();
    Image *apiImage = new ImageOpenGl();

    // this is the constants
    const float MOUSE_SENSITIVITY = 0.1;
    const Bigint WALK_SPEED = Bigint(10);
    Bigint run_speed = Bigint("100");

    // this is the camera, cameras are neat
    Camera *camera = new Camera(res, BigVec3(0.0f, 0.0f, -2.0f));
    camera->rotation.y = 180.0f;

    // this sets up the shader and texture
    Shader *shader = apiShader->makeNewShader("shaders/vertex.glsl", "shaders/fragment.glsl");
    Shader *pointShader = apiShader->makeNewShader("shaders/pointVert.glsl", "shaders/pointFrag.glsl");
    Image *image = apiImage->makeNewImage("assets/textures/FISH.png");

    Font defaultFont("assets/unifont-16.0.04.otf", 48);
    Mesh *mesh2d = new OpenGlMesh(vertices, indices, {2, 2});
    Shader *shader2d = new ShaderOpenGl("shaders/2dVertex.glsl", "shaders/2dFragment.glsl");

    RenderObject2d object2d(mesh2d, shader2d, camera);
    object2d.scale = glm::vec2(48.0f);
    object2d.position.x = 24;
    object2d.position.y = res.y - 24;

    RenderObject::init(pointShader, new OpenGlMesh(), camera);

    delete apiImage;
    delete apiShader;

    LuaHeaders::LuaScriptLib lua;
    lua.include(LuaHeaders::LuaLibEnum::glm);
    lua.include(LuaHeaders::LuaLibEnum::bigvars);
    lua.include(LuaHeaders::LuaLibEnum::objects);

    lua.includeInitialized("shader1", *shader);
    lua.includeInitialized("image1", *image);
    lua.includeInitialized("camera1", *camera);

    lua.includeScripts("./scripts");

    // lua.getFunction("test")();

    // starts running the game loop
    bool running = true;
    Uint64 lastCounter = SDL_GetPerformanceCounter();
    Uint64 currentCounter;
    SDL_Event event;
    float deltaTime;
    const Bigint *speed;

    int accumulatedMouseX = 0;
    int accumulatedMouseY = 0;

    std::ostringstream ss;
    Image *text;

    while (running)
    {
        currentCounter = SDL_GetPerformanceCounter();
        deltaTime = static_cast<float>(currentCounter - lastCounter) / SDL_GetPerformanceFrequency();
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

        RenderObject::updateTime();

        RenderObject::UpdateAllObjects(deltaTime);

        renderingEngine->clearBackground();

        RenderObject::DrawAllObjects();

        // std::cout << "copies: " << Bigint::numCopies << std::endl;
        // Bigint::numCopies = 0;
        // std::cout << "moves: " << Bigint::numMoves << std::endl;
        // Bigint::numMoves = 0;

        ss.str("");
        ss.clear();
        ss << std::fixed << std::setprecision(0) << 1.0f / deltaTime;
        text = new ImageOpenGl(defaultFont.renderText(ss.str(), {255, 255, 255, 255}));

        object2d.Draw(text);

        renderingEngine->swapBuffer();
        delete text;
    }
    // delete everything
    delete shader;
    delete pointShader;
    delete image;
    delete renderingEngine;
    delete camera;
    return 0;
}
