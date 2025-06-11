#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "engine/RenderObject.h"
#include "engine/PhysicsObject.h"
#include "engine/HelperFunctions.hpp"
#include "engine/Camera.hpp"
#include "engine/opengl/HelperFunctionsOpengl.hpp"
#include <string>
#include <memory>
#include <iostream>
// #include <Jolt/Jolt.h>

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
        : RenderObject(shader, slimShady, image, camera, glm::vec3(1.0f), Bigint("384600000000000000000000000"))
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
        cullPriority = CullPriority::High;
    }

    void appendUpdate(const float &deltaTime) override
    {
        rotation.y += deltaTime / 10;
    }
};

int main(int argc, char *argv[])
{
    // it initialises sdl
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);

    // this makes the window
    const glm::vec2 RES{900, 500};
    SDL_Window *window = SDL_CreateWindow("Game", 100, 100, RES.x, RES.y, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << "\n";
        SDL_Quit();
        return 1;
    }
    HelperFunctions *renderingEngine = new HelperFunctionsOpenGl(window);

    // SDL_SetRelativeMouseMode(SDL_TRUE); // hides the mouse
    //  SDL_MaximizeWindow(window);
    glEnable(GL_MULTISAMPLE);
    SDL_GL_SetSwapInterval(0);

    // this is the constants
    const float MOUSE_SENSITIVITY = 0.1;
    const Bigint WALK_SPEED = Bigint(10);
    const Bigint RUN_SPEED = Bigint("1000000000");

    // uuhhh, this is for fun, in case i want to make things a googl meters apart, put whatever number here, see what happens, its pritty cool

    Bigint *pos = getHoweverManyDigits(0);

    // this is the camera, cameras are neat
    Camera *camera = new Camera(RES, *pos, 0.0f, -2.0f);

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
    // cube.velocity.z = 5;

    PhysicsObject cube2(shader, pointShader, image, camera);
    cube2.position.x -= Bigint(10);
    cube2.position.x += *pos;

    PhysicsObject cube3(shader, pointShader, image, camera, glm::vec3(1.0f), 10.0f);
    cube3.position.x += Bigint(10);
    cube3.position.x += *pos;

    const int NUM_TEMPS = 11;
    std::vector<RenderObject *> temp;

    for (int i = 0; i < NUM_TEMPS; i++)
    {
        temp.push_back(new PhysicsObject(shader, pointShader, image, camera, glm::vec3(1.0f), 10.0f));
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
            }
        }

        // gets all keystates
        const Uint8 *keystates = SDL_GetKeyboardState(NULL);

        // if your running, run, otherwise dont
        speed = keystates[SDL_SCANCODE_LSHIFT] ? &RUN_SPEED : &WALK_SPEED;

        // movement
        if (keystates[SDL_SCANCODE_W])
        {
            camera->position += BigVec3(camera->getForwardVector() * deltaTime) * *speed;
        }
        if (keystates[SDL_SCANCODE_S])
        {
            camera->position -= BigVec3(camera->getForwardVector() * deltaTime) * *speed;
        }

        if (keystates[SDL_SCANCODE_D])
        {
            camera->position += BigVec3(camera->getRightVector() * deltaTime) * *speed;
        }
        if (keystates[SDL_SCANCODE_A])
        {
            camera->position -= BigVec3(camera->getRightVector() * deltaTime) * *speed;
        }

        if (keystates[SDL_SCANCODE_SPACE])
        {
            camera->position -= BigVec3(camera->getDownVector() * deltaTime) * *speed;
        }
        if (keystates[SDL_SCANCODE_LCTRL])
        {
            camera->position += BigVec3(camera->getDownVector() * deltaTime) * *speed;
        }

        if (deltaTime > 0)
        {

            camera->yaw -= (accumulatedMouseX / deltaTime) * MOUSE_SENSITIVITY * deltaTime;
            camera->pitch -= (accumulatedMouseY / deltaTime) * MOUSE_SENSITIVITY * deltaTime;

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
