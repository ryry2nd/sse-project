#include <objects/Objects.hpp>
#include <customMath/CustomMath.hpp>
#include <rendering/base/HelperFunctions.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <memory>

std::vector<std::unique_ptr<Rendering::Shader>> shaders;
std::vector<std::unique_ptr<Rendering::Image>> images;
std::vector<std::unique_ptr<Rendering::Mesh>> meshes;
std::vector<std::unique_ptr<Objects::RenderObject>> objects;

extern "C" {
    void setup() {
        std::cout << "example module online\n";
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

    void shutdown() {
        objects.clear();
        meshes.clear();
        shaders.clear();
        images.clear();
    }
}