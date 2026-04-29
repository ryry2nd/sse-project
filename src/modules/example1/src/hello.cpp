// #include <engine/BigObjects/BigObjects.hpp>
#include <engine/Rendering/Rendering.hpp>
#include <engine/CustomMath/CustomMath.hpp>
#include <engine/Rendering/RenderObjects.hpp>

#include <SDL3/SDL_video.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_events.h>
#include <memory>
#include <string>

// const Bigint *speed;
// const Bigint WALK_SPEED = Bigint(10);
// Bigint run_speed = Bigint("100");

static Objects::Scene scene;

#include <random>

float random01()
{
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    return dist(rng);
}

extern "C" {
    void setup() {
        auto &shaders = scene.shaders;
        auto &images = scene.images;
        auto &meshes = scene.meshes;
        auto &buffs = scene.buffs;
        auto &mats = scene.mats;
        auto &params = scene.params;
        auto &cams = scene.cams;
        auto &objs = scene.objs;

        Rendering::CreationFunctions::initAPI("OpenGl4.6");
        glm::vec2 res(900, 500);
        Rendering::sdlWindows.push_back(std::move(Rendering::CreationFunctions::createWindow(res, "Game", SDL_WINDOW_RESIZABLE, 8, false, 0, true)));

        shaders["shader1"] = Rendering::CreationFunctions::createShader((std::string(MODULE_PATH) + "/assets/shaders/floatVert.glsl").c_str(), (std::string(MODULE_PATH) + "/assets/shaders/floatFrag.glsl").c_str());
        shaders["shader2"] = Rendering::CreationFunctions::createShader((std::string(MODULE_PATH) + "/assets/shaders/instanceVert.glsl").c_str(), (std::string(MODULE_PATH) + "/assets/shaders/instanceFrag.glsl").c_str());
        images["fish"] = Rendering::CreationFunctions::createImage((std::string(MODULE_PATH) + "/assets/textures/FISH.png").c_str());
        meshes["cube"] = Rendering::CreationFunctions::createMesh(Objects::cubeVertices, Objects::vertCount, Objects::cubeIndices, Objects::indexCount, (short[]){3,2,3}, 3);

        cams["cam1"] = std::make_unique<Objects::FloatCamera3d>(glm::vec3(0,0,0));
        

        std::unique_ptr<Rendering::Material> mat = std::make_unique<Rendering::Material>();
        mat->shader = shaders["shader1"].get();
        mat->images["im1"] = images["fish"].get();
        mats["mat1"] = std::move(mat);

        std::unique_ptr<Rendering::Material> mat2 = std::make_unique<Rendering::Material>();
        mat2->shader = shaders["shader2"].get();
        mat2->images["im1"] = images["fish"].get();
        mats["mat2"] = std::move(mat2);
        
        

        auto p1 = std::make_unique<Rendering::DrawParams>();
        p1->buffers["Camera"] = cams["cam1"]->buffs["Camera"].get();
        

        
        
        glm::vec3 gridSize(50,50,50);

        std::vector<glm::mat4> models;
        models.reserve(gridSize.x * gridSize.y * gridSize.z);

        for (int x = 0; x < gridSize.x; x++) {
            for (int y = 0; y < gridSize.y; y++) {
                for (int z = 0; z < gridSize.z; z++) {
                    glm::mat4 mat(1.0f);
                    mat = glm::translate(mat, {(x * 3) - ((gridSize.x * 3) / 2), y * 3, ((z * 3) - (gridSize.z * 3)) - 20});
                    models.push_back(mat);
                    // auto cube = std::make_unique<Objects::FloatObject>(meshes["cube"].get(), mats["mat1"].get());
                    // cube->setPos({(x * 3) - ((gridSize.x * 3) / 2), y * 3, ((z * 3) - (gridSize.z * 3)) - 20});

                    // p1->instanceBuffers["Model"].push_back(cube->buffs["Model"].get());

                    // objs["cube_" + std::to_string(x) + "_" + std::to_string(y) + "_" + std::to_string(z)] = std::move(cube);
                }
            }
        }

        auto modsBuff = Rendering::CreationFunctions::createBuff(Rendering::Buff::Type::Storage, Rendering::Buff::Frequency::Dynamic, sizeof(glm::mat4)*models.size(), models.data());

        buffs["modsBuff"] = std::move(modsBuff);

        p1->buffers["Model"] = buffs["modsBuff"].get();

        p1->instanceCount = models.size();

        params["p1"] = std::move(p1);

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
        auto deltaTime = Rendering::Window::deltaTime;
        static float timer = 0.0f;
        static int frameCount = 0;
        static float t = 0.0f;

        t += 10.0f * deltaTime;

        float r = 0.5f + 0.5f * sin(t);
        float g = 0.5f + 0.5f * sin(t + 2.0f);
        float b = 0.5f + 0.5f * sin(t + 4.0f);

        Rendering::sdlWindows[0]->setBackgroundColor({r, g, b, 1.0f});


        frameCount++;
        timer += deltaTime;

        if (timer >= 1.0f)
        {
            double fps = frameCount / timer;
            spdlog::info("FPS: {}", fps);

            frameCount = 0;
            timer = 0.0f;
        }

        

        auto &buffs = scene.buffs;
        auto &mats = scene.mats;
        auto &meshes = scene.meshes;
        auto &params = scene.params;
        auto &cams = scene.cams;
        auto &objs = scene.objs;

        Objects::FloatCamera3d *cam1 = static_cast<Objects::FloatCamera3d*>(cams["cam1"].get());
        cam1->updateBuffs();

        // rotate += 10 * deltaTime;

        // Rendering::DrawParams dps;
        // dps.buffers["Camera"] = cam1->buffs["Camera"].get();

        // for (auto& [key, value] : objs) {
        //     Objects::FloatObject *obj = static_cast<Objects::FloatObject*>(value.get());
        //     obj->setRot({0, rotate,0});
        //     obj->updateBuffs();

            // dps.buffers["Model"] = obj->buffs["Model"].get();

            // Rendering::CreationFunctions::draw(obj->renderPairs[0].mat, obj->renderPairs[0].mesh, &dps);
        // }

        Rendering::CreationFunctions::draw(mats["mat2"].get(), meshes["cube"].get(), params["p1"].get());

        // gets all keystates
        int numKeys;
        const bool *keystates = Rendering::Window::getKeystates(numKeys);

        // if your running, run, otherwise dont
        // speed = keystates[SDL_SCANCODE_LSHIFT] ? &run_speed : &WALK_SPEED;

        // movement
        if (keystates[SDL_SCANCODE_W])
        {
            cam1->movePos(-((cam1->getForwardVector() * deltaTime) * 10.0f));
            // BigObjects::globalCamera.position -= BigVec3(BigObjects::globalCamera.getForwardVector() * deltaTime) * *speed;
        }
        if (keystates[SDL_SCANCODE_S])
        {
            cam1->movePos(((cam1->getForwardVector() * deltaTime) * 10.0f));
            // BigObjects::globalCamera.position += BigVec3(BigObjects::globalCamera.getForwardVector() * deltaTime) * *speed;
        }

        if (keystates[SDL_SCANCODE_D])
        {
            cam1->movePos(-((cam1->getRightVector() * deltaTime) * 10.0f));
            // BigObjects::globalCamera.position -= BigVec3(BigObjects::globalCamera.getRightVector() * deltaTime) * *speed;
        }
        if (keystates[SDL_SCANCODE_A])
        {
            cam1->movePos(((cam1->getRightVector() * deltaTime) * 10.0f));
            // BigObjects::globalCamera.position += BigVec3(BigObjects::globalCamera.getRightVector() * deltaTime) * *speed;
        }

        if (keystates[SDL_SCANCODE_SPACE])
        {
            cam1->movePos(((cam1->getDownVector() * deltaTime) * 10.0f));
            // BigObjects::globalCamera.position += BigVec3(BigObjects::globalCamera.getDownVector() * deltaTime) * *speed;
        }
        if (keystates[SDL_SCANCODE_LCTRL])
        {
            cam1->movePos(-((cam1->getDownVector() * deltaTime) * 10.0f));
            // BigObjects::globalCamera.position -= BigVec3(BigObjects::globalCamera.getDownVector() * deltaTime) * *speed;
        }
    }

    void event(SDL_Event *event, bool *running) {
        Objects::FloatCamera3d *cam1 = static_cast<Objects::FloatCamera3d*>(scene.cams["cam1"].get());

        if (event->type == SDL_EVENT_QUIT)
            *running = false;

        // rotates camera
        if (event->type == SDL_EVENT_MOUSE_MOTION)
        {
            // BigObjects::globalCamera.rotateCamera({event->motion.xrel, event->motion.yrel});
            cam1->rotateCamera({event->motion.xrel, event->motion.yrel}, 0.1f);
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
        scene.clear();
    }
}