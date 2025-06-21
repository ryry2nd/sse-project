#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "engine/objects/Objects.hpp"
#include "engine/HelperFunctions.hpp"
#include <string>
#include <memory>
#include <iostream>

Bigint *getHoweverManyDigits(size_t numZeros)
{
    if (numZeros == 0)
    {
        return new Bigint();
    }
    std::string pos;
    pos.reserve(numZeros + 1);
    pos.append("1");

    for (size_t i = 0; i < numZeros; i++)
    {
        pos.append("0");
    }

    return new Bigint(pos);
}

class Sun : public RenderObject
{
public:
    Sun(Shader *shader, Shader *slimShady, Image *image, Camera *camera)
        : RenderObject(shader, slimShady, image, camera) //, glm::vec3(1.0f), Bigint("384600000000000000000000000"))
    {
        scale *= Bigint("1392000000");
        cullPriority = CullPriority::High;
    }

    void appendUpdate(const float &deltaTime) override
    {
        rotation.y += deltaTime / 10;
    }
};

class Earth : public RenderObject
{
public:
    Earth(Shader *shader, Shader *slimShady, Image *image, Camera *camera)
        : RenderObject(shader, slimShady, image, camera)
    {
        scale *= Bigint("12756000");
        position.y -= (Bigint("12756000") / 2);
        cullPriority = CullPriority::High;
    }

    void appendUpdate(const float &deltaTime) override
    {
        // rotation.y += deltaTime / 10;
    }
};

class MegaCube : public RenderObject
{
public:
    MegaCube(Shader *shader, Shader *slimShady, Image *image, Camera *camera)
        : RenderObject(shader, slimShady, image, camera)
    {
        scale *= 128;
    }

    void appendUpdate(const float &deltaTime) override {}
};

int main(int argc, char *argv[])
{
    glm::vec2 res(900, 500);
    HelperFunctions *renderingEngine = new HelperFunctionsOpenGl(res, "Game", SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE, 8, false, 0);

    // this is the constants
    const float MOUSE_SENSITIVITY = 0.1;
    const Bigint WALK_SPEED = Bigint(10);
    Bigint run_speed = Bigint("100");

    // uuhhh, this is for fun, in case i want to make things a googl meters apart, put whatever number here, see what happens, its pritty cool

    Bigint *pos = getHoweverManyDigits(0);

    // this is the camera, cameras are neat
    Camera *camera = new Camera(res, BigVec3(*pos, 0.0f, -2.0f));
    camera->rotation.y = 180.0f;

    // this sets up the shader and texture
    Shader *shader = new ShaderOpenGl("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");
    Shader *pointShader = new ShaderOpenGl("assets/shaders/pointVert.glsl", "assets/shaders/pointFrag.glsl");
    Image *image = new ImageOpenGl("assets/textures/FISH.png");

    // makes the cubes

    Sun sun(shader, pointShader, image, camera);
    sun.position.x -= Bigint("150000000000");
    sun.position.x += *pos;

    Earth earth(shader, pointShader, image, camera);
    earth.position.x += *pos;

    PhysicsObject cube(shader, pointShader, image, camera);
    cube.position.x += *pos;

    PhysicsObject cube2(shader, pointShader, image, camera);
    cube2.position.x -= Bigint(10);
    cube2.position.x += *pos;

    PhysicsObject cube3(shader, pointShader, image, camera);
    cube3.position.x += Bigint(10);
    cube3.position.x += *pos;

    std::vector<MegaCube *> megaCubes;
    MegaCube *megaCube = nullptr;

    for (int x = 0; x < 10; x++)
    {
        for (int y = 0; y < 10; y++)
        {
            megaCube = new MegaCube(shader, pointShader, image, camera);
            megaCube->position.y += Bigint(200);
            megaCube->position.x += *pos;
            megaCube->position.x += x * 128;
            megaCube->position.z += y * 128;
            megaCubes.push_back(megaCube);
        }
    }

    const int NUM_TEMPS = 11;
    std::vector<RenderObject *> temp;

    for (int i = 0; i < NUM_TEMPS; i++)
    {
        temp.push_back(new PhysicsObject(shader, pointShader, image, camera));
        temp[i]->position.z += Bigint(10 * i);
        temp[i]->position.x += *pos;
    }

    delete pos;
    pos = nullptr;

    // starts running the game loop
    bool running = true;
    Uint64 lastTicks = SDL_GetTicks64();
    Uint64 currentTicks;
    SDL_Event event;
    float deltaTime;
    const Bigint *speed;

    glm::mat4 cameraMatrixTmp;

    int i = 0;

    int accumulatedMouseX = 0;
    int accumulatedMouseY = 0;
    int frames = 0.0f;
    Uint64 start = SDL_GetTicks64();

    while (running)
    {
        currentTicks = SDL_GetTicks64();
        deltaTime = (currentTicks - lastTicks) / 1000.0f;
        lastTicks = currentTicks;

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

        frames++;

        if (SDL_GetTicks64() - start >= 1000)
        {
            std::cout << frames << "\n";
            frames = 0;
            start = SDL_GetTicks64();
        }

        renderingEngine->swapBuffer();
    }
    // delete everything
    delete shader;
    delete pointShader;
    delete image;
    delete renderingEngine;
    delete camera;
    return 0;
}
