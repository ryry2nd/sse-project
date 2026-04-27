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
void FloatCamera::setScl(glm::vec3 scl) {
    FloatParticle::setScl(scl);
    matChanged = true;
}

void FloatCamera::updateBuffs() {
    if (matChanged && buffs[DEFAULT_CAMERA_NAME]) {
        CameraStruct upload({getView(), getProj()});
        buffs[DEFAULT_CAMERA_NAME]->write(0, sizeof(CameraStruct), &upload);
        matChanged = false;
    }
}