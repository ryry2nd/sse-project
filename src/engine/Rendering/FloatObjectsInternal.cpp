#include "FloatObjects.hpp"

#define DEFAULT_CAMERA_NAME "Camera"

using namespace ObjectInternal;

void FloatParticle::setPos(glm::vec3 pos) {
    this->pos = pos;
}
void FloatParticle::setRot(glm::vec3 rot) {
    this->rot = rot;
}
void FloatParticle::setScl(glm::vec3 scl) {
    this->scl = scl;
}
void FloatParticle::movePos(glm::vec3 pos) {
    this->pos += pos;
}

glm::vec3 FloatParticle::getPos() const {
    return pos;
}
glm::vec3 FloatParticle::getRot() const {
    return rot;
}
glm::vec3 FloatParticle::getScl() const {
    return scl;
}

void FloatCamera::setupObject(CameraStruct cameraStruct) {
    buffs[DEFAULT_CAMERA_NAME] = Rendering::CreationFunctions::createBuff(Rendering::Buff::Type::Uniform, Rendering::Buff::Frequency::Dynamic, sizeof(CameraStruct), &cameraStruct);
}

void FloatCamera::setPos(glm::vec3 pos) {
    FloatParticle::setPos(pos);
    matChanged = true;
}
void FloatCamera::setRot(glm::vec3 rot) {
    FloatParticle::setRot(rot);
    matChanged = true;
}
void FloatCamera::movePos(glm::vec3 pos) {
    FloatParticle::movePos(pos);
    matChanged = true;
}

void FloatCamera::rotateCamera(glm::vec2 motion, float mouse_sensitivity) {
        float deltaTime = Rendering::Window::deltaTime;

        if (deltaTime == 0)
        {
            return;
        }
        glm::vec3 rotation;
        rotation.y = getRot().y - ((motion.x / deltaTime) * mouse_sensitivity * deltaTime);
        rotation.x = getRot().x - ((motion.y / deltaTime) * mouse_sensitivity * deltaTime);
        rotation.z = getRot().z;
        setRot(rotation);
}

void FloatCamera::updateBuffs() {
    if (matChanged && buffs[DEFAULT_CAMERA_NAME]) {
        CameraStruct upload({getView(), getProj()});
        buffs[DEFAULT_CAMERA_NAME]->write(0, sizeof(CameraStruct), &upload);
        matChanged = false;
    }
}