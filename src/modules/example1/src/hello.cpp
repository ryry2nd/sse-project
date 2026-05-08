// #include "SDL3/SDL_video.h"
// #include <spdlog/spdlog.h>
// #include <engine/Rendering/Rendering.hpp>
#include <iostream>

extern "C" void setup() {
	std::cout << "module loaded successfully\n";
	// spdlog::info("module loaded successfully");
	// Rendering::CreationFunctions::initAPI("OpenGl4.6");
	// glm::vec2 res(900, 500);
	// Rendering::sdlWindows.push_back(std::move(Rendering::CreationFunctions::createWindow(res, "Game", SDL_WINDOW_RESIZABLE, 8, false, 0, true)));
}

// // #include <engine/BigObjects/BigObjects.hpp>
// #include "SDL3/SDL_init.h"
// #include <engine/Rendering/Rendering.hpp>
// // #include <engine/CustomMath/CustomMath.hpp>
// #include <engine/Rendering/RenderObjects.hpp>

// #include <SDL3/SDL_video.h>
// #include <SDL3/SDL_keycode.h>
// #include <SDL3/SDL_events.h>
// #include <memory>
// #include <string>

// #ifndef MODULE_PATH
//     #define MODULE_PATH ""
// #endif

// // const Bigint *speed;
// // const Bigint WALK_SPEED = Bigint(10);
// // Bigint run_speed = Bigint("100");

// static Objects::Scene scene;

// static bool outputFPS = false;

// #include <random>

// float random01()
// {
//     static std::mt19937 rng(std::random_device{}());
//     static std::uniform_real_distribution<float> dist(0.0f, 1.0f);

//     return dist(rng);
// }

// extern "C" {
//     void setup() {
//         auto &shaders = scene.shaders;
//         auto &images = scene.images;
//         auto &meshes = scene.meshes;
//         auto &buffs = scene.buffs;
//         auto &mats = scene.mats;
//         auto &params = scene.params;
//         auto &cams = scene.cams;
//         auto &objs = scene.objs;

// 		SDL_Init(SDL_INIT_VIDEO);
//         Rendering::CreationFunctions::initAPI("OpenGl4.6");
//         glm::vec2 res(900, 500);
//         Rendering::sdlWindows.push_back(std::move(Rendering::CreationFunctions::createWindow(res, "Game", SDL_WINDOW_RESIZABLE, 8, false, 0, true)));

//         shaders["shader1"] = Rendering::CreationFunctions::createShader(std::string(MODULE_PATH) + "/assets/shaders/floatCube.slang");
//         shaders["shader2"] = Rendering::CreationFunctions::createShader(std::string(MODULE_PATH) + "/assets/shaders/instanceCube.slang");
//         images["fish"] = Rendering::CreationFunctions::createImage((std::string(MODULE_PATH) + "/assets/textures/FISH.png").c_str());
//         meshes["cube"] = Rendering::CreationFunctions::createMesh(Objects::cubeVertices, Objects::vertCount, Objects::cubeIndices, Objects::indexCount, (short[]){3,2,3}, 3);

//         cams["cam1"] = std::make_unique<Objects::FloatCamera3d>(glm::vec3(0,0,0));


//         std::unique_ptr<Rendering::Material> mat = std::make_unique<Rendering::Material>();
//         mat->shader = shaders["shader1"].get();
//         mat->images[0] = images["fish"].get();
//         mats["mat1"] = std::move(mat);

//         std::unique_ptr<Rendering::Material> mat2 = std::make_unique<Rendering::Material>();
//         mat2->shader = shaders["shader2"].get();
//         mat2->images[0] = images["fish"].get();
//         mats["mat2"] = std::move(mat2);


//         auto p1 = std::make_unique<Rendering::DrawParams>();
//         p1->ubo[0] = cams["cam1"]->buffs["Camera"].get();

//         auto p2 = std::make_unique<Rendering::DrawParams>();
//         p2->ubo[0] = cams["cam1"]->buffs["Camera"].get();

//         glm::mat4 cubeMat(1.0f);
//         cubeMat = glm::translate(cubeMat, {-10, 0, 0});
//         buffs["cube_view"] = Rendering::CreationFunctions::createBuff(Rendering::Buff::Type::Uniform, Rendering::Buff::Frequency::Dynamic, sizeof(glm::mat4), &cubeMat);
//         p2->ssbo[0] = buffs["cube_view"].get();

//         params["p2"] = std::move(p2);

//         glm::vec3 gridSize(50,50,50);

//         std::vector<glm::mat4> models;
//         models.reserve(gridSize.x * gridSize.y * gridSize.z);

//         for (int x = 0; x < gridSize.x; x++) {
//             for (int y = 0; y < gridSize.y; y++) {
//                 for (int z = 0; z < gridSize.z; z++) {
//                     glm::mat4 mat(1.0f);
//                     mat = glm::translate(mat, {(x * 3) - ((gridSize.x * 3) / 2), y * 3, ((z * 3) - (gridSize.z * 3)) - 20});
//                     models.push_back(mat);
//                 }
//             }
//         }

//         auto modsBuff = Rendering::CreationFunctions::createBuff(Rendering::Buff::Type::Storage, Rendering::Buff::Frequency::Static, sizeof(glm::mat4)*models.size(), models.data());

//         buffs["modsBuff"] = std::move(modsBuff);

//         p1->ssbo[0] = buffs["modsBuff"].get();

//         p1->instanceCount = models.size();

//         p1->settings = Rendering::DrawParams::Settings::EnableFBO;

//         auto fb = Rendering::CreationFunctions::createFrameBuffer(res, Rendering::FrameBuffer::Settings::Color);
//         p1->fbo = fb.get();

//         std::unique_ptr<Rendering::Material> mat3 = std::make_unique<Rendering::Material>();
//         mat3->shader = shaders["shader1"].get();
//         mat3->images[0] = fb->getColorImage();
//         mats["mat3"] = std::move(mat3);

//         scene.fbos["fbo1"] = std::move(fb);

//         params["p1"] = std::move(p1);

//         // Bigint pos = Bigint::getHoweverManyDigits(0);
//         // BigObjects::globalCamera.position.x = BigObjects::globalCamera.position.x + pos;
//         // BigObjects::globalCamera.position.z = BigObjects::globalCamera.position.z - Bigint(10);
//         // BigObjects::globalCamera.rotation.y = BigObjects::globalCamera.rotation.y + 180;

//         // bigObjects.push_back(std::make_unique<BigObjects::Singularity>(meshes[0].get()));
//         // bigObjects[bigObjects.size()-1]->position.x += pos;
//         // bigObjects.push_back(std::make_unique<BigObjects::Singularity>(meshes[0].get()));
//         // bigObjects[bigObjects.size()-1]->position.x -= Bigint(10);
//         // bigObjects[bigObjects.size()-1]->position.x += pos;
//         // bigObjects.push_back(std::make_unique<BigObjects::Singularity>(meshes[0].get()));
//         // bigObjects[bigObjects.size()-1]->position.x += Bigint(10);
//         // bigObjects[bigObjects.size()-1]->position.x += pos;

//         // bigObjects.push_back(std::make_unique<BigObjects::Singularity>(meshes[2].get()));
//         // bigObjects[bigObjects.size()-1]->position.x -= Bigint("150000000000");
//         // bigObjects[bigObjects.size()-1]->position.x += pos;
//         // bigObjects.push_back(std::make_unique<BigObjects::Singularity>(meshes[1].get()));
//         // bigObjects[bigObjects.size()-1]->position.y -= Bigint("6378000");
//         // bigObjects[bigObjects.size()-1]->position.x += pos;

//         // int size = 11;
//         // for (int i = 0; i < size; i++) {
//         //     std::unique_ptr<BigObjects::Singularity> lineObject = std::make_unique<BigObjects::Singularity>(meshes[0].get());
//         //     lineObject->position.z = lineObject->position.z + Bigint(10 * (i+1));
//         //     lineObject->position.x = lineObject->position.x + pos;
//         //     bigObjects.push_back(std::move(lineObject));
//         // }
//     }

//     // void loop() {
//     //     static float rotate = 0;
//     //     auto deltaTime = Rendering::Window::deltaTime;
//     //     static float timer = 0.0f;
//     //     static int frameCount = 0;
//     //     static float t = 0.0f;

//     //     t += deltaTime * 1.0f;

//     //     float r = 0.5f + 0.3f * sin(t);
//     //     float g = 0.5f + 0.3f * sin(t + 2.0f);
//     //     float b = 0.5f + 0.3f * sin(t + 4.0f);

//     //     // makes background look like a 70s disco rave while under 20 pounds of lsd
//     //     Rendering::sdlWindows[0]->setBackgroundColor({r, g, b, 1.0f});


//     //     frameCount++;
//     //     timer += deltaTime;

//     //     if (timer >= 1.0f)
//     //     {
//     //         if (outputFPS) {
//     //             double fps = frameCount / timer;
//     //             spdlog::info("FPS: {:.2f}", fps);
//     //         }

//     //         frameCount = 0;
//     //         timer = 0.0f;
//     //     }

//     //     auto &buffs = scene.buffs;
//     //     auto &mats = scene.mats;
//     //     auto &meshes = scene.meshes;
//     //     auto &params = scene.params;
//     //     auto &cams = scene.cams;
//     //     auto &objs = scene.objs;

//     //     Objects::FloatCamera3d *cam1 = static_cast<Objects::FloatCamera3d*>(cams["cam1"].get());
//     //     cam1->updateBuffs();

//     //     params["p1"]->fbo->getColorImage()->clearTransparent();
//     //     Rendering::CreationFunctions::draw(mats["mat2"].get(), meshes["cube"].get(), params["p1"].get());

//     //     // Rendering::CreationFunctions::draw(mats["mat3"].get(), meshes["cube"].get(), params["p2"].get());

//     //     // gets all keystates
//     //     int numKeys;
//     //     const bool *keystates = Rendering::Window::getKeystates(numKeys);

//     //     // if your running, run, otherwise dont
//     //     // speed = keystates[SDL_SCANCODE_LSHIFT] ? &run_speed : &WALK_SPEED;

//     //     // movement
//     //     if (keystates[SDL_SCANCODE_W])
//     //     {
//     //         cam1->movePos(-((cam1->getForwardVector() * deltaTime) * 10.0f));
//     //         // BigObjects::globalCamera.position -= BigVec3(BigObjects::globalCamera.getForwardVector() * deltaTime) * *speed;
//     //     }
//     //     if (keystates[SDL_SCANCODE_S])
//     //     {
//     //         cam1->movePos(((cam1->getForwardVector() * deltaTime) * 10.0f));
//     //         // BigObjects::globalCamera.position += BigVec3(BigObjects::globalCamera.getForwardVector() * deltaTime) * *speed;
//     //     }

//     //     if (keystates[SDL_SCANCODE_D])
//     //     {
//     //         cam1->movePos(-((cam1->getRightVector() * deltaTime) * 10.0f));
//     //         // BigObjects::globalCamera.position -= BigVec3(BigObjects::globalCamera.getRightVector() * deltaTime) * *speed;
//     //     }
//     //     if (keystates[SDL_SCANCODE_A])
//     //     {
//     //         cam1->movePos(((cam1->getRightVector() * deltaTime) * 10.0f));
//     //         // BigObjects::globalCamera.position += BigVec3(BigObjects::globalCamera.getRightVector() * deltaTime) * *speed;
//     //     }

//     //     if (keystates[SDL_SCANCODE_SPACE])
//     //     {
//     //         cam1->movePos(((cam1->getDownVector() * deltaTime) * 10.0f));
//     //         // BigObjects::globalCamera.position += BigVec3(BigObjects::globalCamera.getDownVector() * deltaTime) * *speed;
//     //     }
//     //     if (keystates[SDL_SCANCODE_LCTRL])
//     //     {
//     //         cam1->movePos(-((cam1->getDownVector() * deltaTime) * 10.0f));
//     //         // BigObjects::globalCamera.position -= BigVec3(BigObjects::globalCamera.getDownVector() * deltaTime) * *speed;
//     //     }
//     // }

//     // void event(SDL_Event *event, bool *running) {
//     //     Objects::FloatCamera3d *cam1 = static_cast<Objects::FloatCamera3d*>(scene.cams["cam1"].get());

//     //     if (event->type == SDL_EVENT_QUIT)
//     //         *running = false;

//     //     // rotates camera
//     //     if (event->type == SDL_EVENT_MOUSE_MOTION)
//     //     {
//     //         // BigObjects::globalCamera.rotateCamera({event->motion.xrel, event->motion.yrel});
//     //         cam1->rotateCamera({event->motion.xrel, event->motion.yrel}, 0.1f);
//     //     }

//     //     if (event->type == SDL_EVENT_KEY_DOWN)
//     //     {
//     //         SDL_Keycode key = event->key.key;
//     //         if (key == SDLK_ESCAPE)
//     //         {
//     //             *running = false;
//     //         }
//     //         if (key == SDLK_Z)
//     //         {
//     //             // run_speed *= Bigint(10);
//     //         }
//     //         if (key == SDLK_X)
//     //         {
//     //             // run_speed /= Bigint(10);
//     //         }
//     //         if (key == SDLK_F3) {
//     //             outputFPS = !outputFPS;
//     //         }
//     //     }
//     //     if (event->type == SDL_EVENT_WINDOW_RESIZED)
//     //     {
//     //         Rendering::sdlWindows[0]->updateScreenRes();
//     //     }
//     // }

//     void shutdown() {
//         scene.clear();
//     }
// }