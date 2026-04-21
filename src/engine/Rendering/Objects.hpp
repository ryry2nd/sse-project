#pragma once

#include "Rendering.hpp"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

namespace Objects {
    typedef size_t ObjectID;

    class Object {
    public:
        virtual ~Object() = default;
    };

    class Drawable : public Object {
    public:
        std::vector<Rendering::StorageBuff *> buffs;
        virtual void Draw() = 0;
    };

    class Camera : public Object {
    public:
        std::vector<Drawable *> allDraws;
        Rendering::FrameBuff *frameBuffer;

        virtual void Render() = 0;
    };

    inline std::vector<std::unique_ptr<Object>> allObjects = []() {
        std::vector<std::unique_ptr<Object>> v;
        v.reserve(OBJECT_PREALLOC);
        return v;
    }();

    inline std::vector<Camera*> allCams = []() {
        std::vector<Camera*> v;
        v.reserve(CAMERA_PREALLOC);
        return v;
    }();

    template<typename T>
    void addParticle(std::unique_ptr<T> obj);
    void removeParticle(ObjectID id);

    class FloatParticle : public Object {
    public:
        glm::vec3 position;
    };
}