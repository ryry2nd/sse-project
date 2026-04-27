// #include <engine/BigObjects/BigObjects.hpp>
#include "engine/Rendering/Rendering.hpp"
#include <engine/CustomMath/CustomMath.hpp>
#include <engine/Rendering/RenderObjects.hpp>
// #include <iostream>
// #include <vector>
// #include <string>
// #include <memory>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_events.h>
#include <memory>

std::vector<std::unique_ptr<Rendering::Shader>> shaders;
std::vector<std::unique_ptr<Rendering::Image>> images;
std::vector<std::unique_ptr<Rendering::Mesh>> meshes;
std::vector<std::unique_ptr<Rendering::Buff>> buffs;
std::vector<Rendering::Material*> mats;
std::vector<Rendering::DrawParams*> params;
// std::vector<std::unique_ptr<BigObjects::Singularity>> bigObjects;

// const Bigint *speed;
// const Bigint WALK_SPEED = Bigint(10);
// Bigint run_speed = Bigint("100");

struct Camera {
    glm::mat4 view;
    glm::mat4 proj;
};

glm::vec3 camPos;

extern "C" {
    void setup() {
        Rendering::CreationFunctions::initAPI("OpenGl4.6");
        glm::vec2 res(900, 500);
        Rendering::sdlWindows.push_back(std::move(Rendering::CreationFunctions::createWindow(res, "Game", SDL_WINDOW_RESIZABLE, 8, false, 0, true)));

        shaders.push_back(Rendering::CreationFunctions::createShader((std::string(MODULE_PATH) + "/assets/shaders/debugVert.glsl").c_str(), (std::string(MODULE_PATH) + "/assets/shaders/debugFrag.glsl").c_str()));
        images.push_back(Rendering::CreationFunctions::createImage((std::string(MODULE_PATH) + "/assets/textures/FISH.png").c_str()));
        meshes.push_back(Rendering::CreationFunctions::createMesh(Objects::cubeVertices, Objects::vertCount, Objects::cubeIndices, Objects::indexCount, (short[]){3,2,3}, 3));


        Camera cam = Camera();
        cam.view = glm::translate(glm::mat4(1.0f), -camPos);
        cam.proj = glm::perspective(glm::radians(70.0f), res.x/res.y, 0.1f, 100.0f);
        buffs.push_back(Rendering::CreationFunctions::createBuff(Rendering::Buff::Type::Uniform, Rendering::Buff::Frequency::Dynamic, sizeof(Camera), &cam));

        static glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -3));
        buffs.push_back(Rendering::CreationFunctions::createBuff(Rendering::Buff::Type::Uniform, Rendering::Buff::Frequency::Dynamic, sizeof(glm::mat4), &model));

        Rendering::Material *mat = new Rendering::Material();
        mat->shader = shaders[0].get();
        mat->images["im1"] = images[0].get();

        Rendering::DrawParams *pr = new Rendering::DrawParams();
        pr->buffers["Camera"] = buffs[0].get();
        pr->buffers["pos"] = buffs[1].get();
        

        mats.push_back(mat);
        params.push_back(pr);

        // Bigint pos = Bigint::getHoweverManyDigits(0);
        // BigObjects::globalCamera.position.x = BigObjects::globalCamera.position.x + pos;
        // BigObjects::globalCamera.position.z = BigObjects::globalCamera.position.z - Bigint(10);
        // BigObjects::globalCamera.rotation.y = BigObjects::globalCamera.rotation.y + 180;

        // bigObjects.push_back(std::make_unique<BigObjects::Singularity>(meshes[0].get()));
        // bigObjects[bigObjects.size()-1]->position.x += pos;
        // bigObjects.push_back(std::make_unique<BigObjects::Singularity>(meshes[0].get()));
        // bigObjects[bigObjects.size()-1]->position.x -= Bigint(10);
        // bigObjects[bigObjects.size()-1]->position.x += pos;
        // bigObjects.push_back(std::make_unique<BigObjects::Singularity>(meshes[0].get()));
        // bigObjects[bigObjects.size()-1]->position.x += Bigint(10);
        // bigObjects[bigObjects.size()-1]->position.x += pos;

        // bigObjects.push_back(std::make_unique<BigObjects::Singularity>(meshes[2].get()));
        // bigObjects[bigObjects.size()-1]->position.x -= Bigint("150000000000");
        // bigObjects[bigObjects.size()-1]->position.x += pos;
        // bigObjects.push_back(std::make_unique<BigObjects::Singularity>(meshes[1].get()));
        // bigObjects[bigObjects.size()-1]->position.y -= Bigint("6378000");
        // bigObjects[bigObjects.size()-1]->position.x += pos;

        // int size = 11;
        // for (int i = 0; i < size; i++) {
        //     std::unique_ptr<BigObjects::Singularity> lineObject = std::make_unique<BigObjects::Singularity>(meshes[0].get());
        //     lineObject->position.z = lineObject->position.z + Bigint(10 * (i+1));
        //     lineObject->position.x = lineObject->position.x + pos;
        //     bigObjects.push_back(std::move(lineObject));
        // }
    }

    void loop() {
        static float rotate = 0;
        const float &deltaTime = Rendering::Window::deltaTime;

        rotate += 10 * deltaTime;

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0, 0, -3));
        model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 1.0f, 0.0f));
        
        
        glm::mat4 view = glm::translate(glm::mat4(1.0f), -camPos);

        buffs[0]->write(0, sizeof(glm::mat4), &view);
        buffs[1]->write(0, sizeof(glm::mat4), &model);

        Rendering::CreationFunctions::draw(mats[0], meshes[0].get(), params[0]);

        // gets all keystates
        int numKeys;
        const bool *keystates = Rendering::Window::getKeystates(numKeys);

        // if your running, run, otherwise dont
        // speed = keystates[SDL_SCANCODE_LSHIFT] ? &run_speed : &WALK_SPEED;

        // movement
        if (keystates[SDL_SCANCODE_W])
        {
            camPos.z -= 10 * deltaTime;
            // BigObjects::globalCamera.position -= BigVec3(BigObjects::globalCamera.getForwardVector() * deltaTime) * *speed;
        }
        if (keystates[SDL_SCANCODE_S])
        {
            camPos.z += 10 * deltaTime;
            // BigObjects::globalCamera.position += BigVec3(BigObjects::globalCamera.getForwardVector() * deltaTime) * *speed;
        }

        if (keystates[SDL_SCANCODE_D])
        {
            // BigObjects::globalCamera.position -= BigVec3(BigObjects::globalCamera.getRightVector() * deltaTime) * *speed;
        }
        if (keystates[SDL_SCANCODE_A])
        {
            // BigObjects::globalCamera.position += BigVec3(BigObjects::globalCamera.getRightVector() * deltaTime) * *speed;
        }

        if (keystates[SDL_SCANCODE_SPACE])
        {
            // BigObjects::globalCamera.position += BigVec3(BigObjects::globalCamera.getDownVector() * deltaTime) * *speed;
        }
        if (keystates[SDL_SCANCODE_LCTRL])
        {
            // BigObjects::globalCamera.position -= BigVec3(BigObjects::globalCamera.getDownVector() * deltaTime) * *speed;
        }
    }

    void event(SDL_Event *event, bool *running) {
        if (event->type == SDL_EVENT_QUIT)
            *running = false;

        // rotates camera
        if (event->type == SDL_EVENT_MOUSE_MOTION)
        {
            // BigObjects::globalCamera.rotateCamera({event->motion.xrel, event->motion.yrel});
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
                // run_speed *= Bigint(10);
            }
            if (key == SDLK_X)
            {
                // run_speed /= Bigint(10);
            }
        }
        if (event->type == SDL_EVENT_WINDOW_RESIZED)
        {
            Rendering::sdlWindows[0]->updateScreenRes();
        }
    }

    void shutdown() {
        delete mats[0];
        delete params[0];

        // bigObjects.clear();
        meshes.clear();
        shaders.clear();
        images.clear();
    }
}