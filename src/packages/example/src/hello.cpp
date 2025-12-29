#include <iostream>
#include <objects/Objects.hpp>
#include <customMath/CustomMath.hpp>
#include <rendering/base/HelperFunctions.hpp>
#include <vector>
#include <string>

std::vector<Rendering::Shader *> shaders;
std::vector<Rendering::Image *> images;
std::vector<Rendering::Mesh *> meshes;
std::vector<Objects::RenderObject *> objects;

extern "C" {
    void setup() {
        Rendering::Shader *shader = Rendering::defaultShaderAPI->makeNewShader((std::string(MODULE_PATH) + "/assets/shaders/vertex.glsl").c_str(), (std::string(MODULE_PATH) + "/assets/shaders/fragment.glsl").c_str());
        Rendering::Image *image = Rendering::defaultImageAPI->makeNewImage((std::string(MODULE_PATH) + "/assets/textures/FISH.png").c_str());
        Rendering::Mesh *mesh = Rendering::defaultMeshAPI->makeNewMesh(shader, Objects::cubeVertices, Objects::cubeIndices, {3,2,3});
        Rendering::Mesh *earthMesh = Rendering::defaultMeshAPI->makeNewMesh(shader, Objects::cubeVertices, Objects::cubeIndices, {3,2,3});
        earthMesh->sizeOffset *= 12756000;
        Rendering::Mesh *sunMesh = Rendering::defaultMeshAPI->makeNewMesh(shader, Objects::cubeVertices, Objects::cubeIndices, {3,2,3});
        sunMesh->sizeOffset *= 1392000000;

        shaders.push_back(shader);
        images.push_back(image);
        meshes.push_back(mesh);
        meshes.push_back(earthMesh);
        meshes.push_back(sunMesh);
        
        Bigint pos = Bigint::getHoweverManyDigits(0);
        Objects::globalCamera.position.x = Objects::globalCamera.position.x + pos;
        Objects::globalCamera.position.z = Objects::globalCamera.position.z - Bigint(10);
        Objects::globalCamera.rotation.y = Objects::globalCamera.rotation.y + 180;

        Objects::RenderObject *renderObject = new Objects::RenderObject(mesh);
        renderObject->position.x += pos;
        Objects::RenderObject *renderObject2 = new Objects::RenderObject(mesh);
        renderObject2->position.x -= Bigint(10);
        renderObject2->position.x += pos;
        Objects::RenderObject *renderObject3 = new Objects::RenderObject(mesh);
        renderObject3->position.x += Bigint(10);
        renderObject3->position.x += pos;

        Objects::RenderObject *sunObject = new Objects::RenderObject(sunMesh);
        sunObject->position.x -= Bigint("150000000000");
        sunObject->position.x += pos;
        Objects::RenderObject *earthObject = new Objects::RenderObject(earthMesh);
        earthObject->position.y -= Bigint("6378000");
        earthObject->position.x += pos;

        objects.push_back(renderObject);
        objects.push_back(renderObject2);
        objects.push_back(renderObject3);
        objects.push_back(sunObject);
        objects.push_back(earthObject);

        int size = 11;
        for (int i = 0; i < size; i++) {
            Objects::RenderObject *lineObject = new Objects::RenderObject(mesh);
            lineObject->position.z = lineObject->position.z + Bigint(10 * (i+1));
            lineObject->position.x = lineObject->position.x + pos;
            objects.push_back(lineObject);
        }
    }

    void shutdown() {
        for (auto *foo : shaders) {
            delete foo;
        }
        for (auto *foo : images) {
            delete foo;
        }
        for (auto *foo : meshes) {
            delete foo;
        }
        for (auto *foo : objects) {
            delete foo;
        }
    }
}