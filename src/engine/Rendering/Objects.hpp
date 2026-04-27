#pragma once

#include "Rendering.hpp"

#include <unordered_set>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

namespace Objects {
    enum RenderLayer : uint32_t {
        World      = 1 << 0,
        UI         = 1 << 1,
        Debug      = 1 << 2,
        Invisible  = 1 << 3
    };

    class Particle {
    public:
        virtual ~Particle() = default;
    };

    class Camera : public Particle {
    public:
        struct CameraStruct {
            glm::mat4 view;
            glm::mat4 proj;
        };
        
        std::unordered_map<std::string, std::unique_ptr<Rendering::Buff>> buffs;
        uint32_t viewMask = RenderLayer::World;
    };

    class Object : public Particle {
    public:
        struct RenderPair {
            Rendering::Mesh* mesh;
            Rendering::Material *mat;
        };

        virtual ~Object() = default;

        virtual void updateBuffs() = 0;

        std::unordered_map<std::string, std::unique_ptr<Rendering::Buff>> buffs;
        std::vector<RenderPair> renderPairs;
        RenderLayer layer = RenderLayer::World;
        std::unordered_set<const Camera*> hiddenFromCameras;
    };

    struct Scene {
        std::vector<std::unique_ptr<Camera>> cams;
        std::vector<std::unique_ptr<Object>> draws;
        std::vector<std::unique_ptr<Rendering::Mesh>> meshes;
        std::vector<std::unique_ptr<Rendering::Shader>> shaders;
        std::vector<std::unique_ptr<Rendering::Buff>> buffs;
        std::vector<std::unique_ptr<Rendering::Image>> images;
        std::vector<std::unique_ptr<Rendering::Material>> mats;
        std::vector<std::unique_ptr<Rendering::DrawParams>> dps;
        void drawScene();
    };

    extern const float cubeVertices[];
    extern const unsigned int cubeIndices[];
    extern const float vertices2d[];
    extern const unsigned int indices2d[];

    extern const size_t vertCount;
    extern const size_t indexCount;
    extern const size_t vert2dCount;
    extern const size_t ind2dCount;
}