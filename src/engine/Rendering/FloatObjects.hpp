#pragma once

#include "Objects.hpp"
#include "Rendering.hpp"

namespace ObjectInternal {
    class FloatParticle {
    public:
        virtual void setPos(glm::vec3 pos);
        virtual void setRot(glm::vec3 rot);
        virtual void setScl(glm::vec3 scl);

        glm::vec3 getPos() const;
        glm::vec3 getRot() const;
        glm::vec3 getScl() const;
    private:
        glm::vec3 pos;
        glm::vec3 rot;
        glm::vec3 scl;
    };

    class FloatCamera : public FloatParticle, public Objects::Camera {
    public:
        virtual glm::mat4 getProj() const = 0;
        virtual glm::mat4 getView() const = 0;
        virtual void updateBuffs();

        void setPos(glm::vec3 pos);
        void setRot(glm::vec3 rot);
        void setScl(glm::vec3 scl);
    protected:
        void setupObject(CameraStruct camStruct);
    private:
        bool matChanged = false;
    };
}

namespace Objects {
    class FloatObject : public ObjectInternal::FloatParticle, public Object {
        FloatObject();
        FloatObject(Rendering::Mesh **mesh, Rendering::Material **mat, size_t size=1);

        void setPos(glm::vec3 pos);
        void setRot(glm::vec3 rot);
        void setScl(glm::vec3 scl);

        virtual void updateBuffs() = 0;
    private:
        void setupObject();
        bool modelChanged = false;
    };

    class FloatCamera2d : public ObjectInternal::FloatCamera {
    public:
        FloatCamera2d();

        float nearPlane = -1.0f;
        float farPlane = 1.0f;

        glm::mat4 getProj() const;
        glm::mat4 getView() const;
    };

    class FloatCamera3d : public ObjectInternal::FloatCamera {
    public:
        FloatCamera3d();

        glm::vec3 getForwardVector() const;
        glm::vec3 getRightVector() const;
        glm::vec3 getDownVector() const;

        glm::mat4 getProj() const;
        glm::mat4 getView() const;

        float fov = 70.0f;
        float nearPlane = 0.1f;
        float farPlane = 100.0f;
    };
}