#include <objects/Objects.hpp>
#include <customMath/CustomMath.hpp>
#include <rendering/base/Rendering.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_events.h>

std::vector<std::unique_ptr<Rendering::Shader>> shaders;
std::vector<std::unique_ptr<Rendering::Image>> images;
std::vector<std::unique_ptr<Rendering::Mesh>> meshes;
std::vector<std::unique_ptr<Objects::RenderObject>> objects;

const Bigint *speed;
const Bigint WALK_SPEED = Bigint(10);
Bigint run_speed = Bigint("100");

extern "C" {
    void setup() {
        std::cout << "example module online\n";

        Rendering::CreationFunctions::initAPI("OpenGl4.6");
        Rendering::sdlWindows.push_back(Rendering::CreationFunctions::createWindow({900, 500}, "Game", SDL_WINDOW_RESIZABLE, 8, false, 0, true));

        shaders.push_back(Rendering::CreationFunctions::createShader((std::string(MODULE_PATH) + "/assets/shaders/vertex.glsl").c_str(), (std::string(MODULE_PATH) + "/assets/shaders/fragment.glsl").c_str()));
        images.push_back(Rendering::CreationFunctions::createImage((std::string(MODULE_PATH) + "/assets/textures/FISH.png").c_str()));
        meshes.push_back(Rendering::CreationFunctions::createMesh(shaders[0].get(), Objects::cubeVertices, Objects::cubeIndices, {3,2,3}));
        meshes.push_back(Rendering::CreationFunctions::createMesh(shaders[0].get(), Objects::cubeVertices, Objects::cubeIndices, {3,2,3}));
        meshes[meshes.size()-1]->sizeOffset *= 12756000;
        meshes.push_back(Rendering::CreationFunctions::createMesh(shaders[0].get(), Objects::cubeVertices, Objects::cubeIndices, {3,2,3}));
        meshes[meshes.size()-1]->sizeOffset *= 1392000000;

        meshes[0]->images.push_back(images[0].get());
        meshes[1]->images.push_back(images[0].get());
        meshes[2]->images.push_back(images[0].get());
        
        Bigint pos = Bigint::getHoweverManyDigits(0);
        Objects::globalCamera.position.x = Objects::globalCamera.position.x + pos;
        Objects::globalCamera.position.z = Objects::globalCamera.position.z - Bigint(10);
        Objects::globalCamera.rotation.y = Objects::globalCamera.rotation.y + 180;

        objects.push_back(std::make_unique<Objects::RenderObject>(meshes[0].get()));
        objects[objects.size()-1]->position.x += pos;
        objects.push_back(std::make_unique<Objects::RenderObject>(meshes[0].get()));
        objects[objects.size()-1]->position.x -= Bigint(10);
        objects[objects.size()-1]->position.x += pos;
        objects.push_back(std::make_unique<Objects::RenderObject>(meshes[0].get()));
        objects[objects.size()-1]->position.x += Bigint(10);
        objects[objects.size()-1]->position.x += pos;

        objects.push_back(std::make_unique<Objects::RenderObject>(meshes[2].get()));
        objects[objects.size()-1]->position.x -= Bigint("150000000000");
        objects[objects.size()-1]->position.x += pos;
        objects.push_back(std::make_unique<Objects::RenderObject>(meshes[1].get()));
        objects[objects.size()-1]->position.y -= Bigint("6378000");
        objects[objects.size()-1]->position.x += pos;

        int size = 11;
        for (int i = 0; i < size; i++) {
            std::unique_ptr<Objects::RenderObject> lineObject = std::make_unique<Objects::RenderObject>(meshes[0].get());
            lineObject->position.z = lineObject->position.z + Bigint(10 * (i+1));
            lineObject->position.x = lineObject->position.x + pos;
            objects.push_back(std::move(lineObject));
        }
    }

    void loop() {
        // gets all keystates
        int numKeys;
        const bool *keystates = Rendering::Window::getKeystates(numKeys);
        const float &deltaTime = Rendering::Window::deltaTime;

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
    }

    void event(SDL_Event *event, bool *running) {
        if (event->type == SDL_EVENT_QUIT)
            *running = false;

        // rotates camera
        if (event->type == SDL_EVENT_MOUSE_MOTION)
        {
            Objects::globalCamera.rotateCamera({event->motion.xrel, event->motion.yrel});
        }

        if (event->type == SDL_EVENT_KEY_DOWN)
        {
            SDL_Keycode key = event->key.key;
            if (key == SDLK_ESCAPE)
            {
                *running = false;
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
        if (event->type == SDL_EVENT_WINDOW_RESIZED)
        {
            Rendering::sdlWindows[0]->updateScreenRes();
        }
    }

    void shutdown() {
        objects.clear();
        meshes.clear();
        shaders.clear();
        images.clear();
    }
}