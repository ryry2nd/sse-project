#include "FloatObjects.hpp"
#include "Rendering.hpp"
#include <spdlog/spdlog.h>

using namespace Objects;

#define DEFAULT_MODEL_NAME "Model"

void FloatObject::setupObject() {
    glm::mat4 mat(1.0f);
    buffs[DEFAULT_MODEL_NAME] = Rendering::CreationFunctions::createBuff(Rendering::Buff::Type::Uniform, Rendering::Buff::Frequency::Dynamic, sizeof(glm::mat4), &mat);
}

FloatObject::FloatObject() {
    setupObject();
}

FloatObject::FloatObject(Rendering::Mesh *mesh, Rendering::Material *mat) {
    setupObject();
    renderPairs.push_back({mesh, mat});
}

FloatObject::FloatObject(Rendering::Mesh **mesh, Rendering::Material **mat, size_t size) {
    setupObject();
    renderPairs.reserve(size);

    for (size_t i = 0; i < size; i++)
        renderPairs.push_back({mesh[i], mat[i]});
}

void FloatObject::setPos(glm::vec3 pos) {
    FloatParticle::setPos(pos);
    modelChanged = true;
}
void FloatObject::setRot(glm::vec3 rot) {
    FloatParticle::setRot(rot);
    modelChanged = true;
}
void FloatObject::setScl(glm::vec3 rot) {
    FloatParticle::setScl(rot);
    modelChanged = true;
}

void FloatObject::updateBuffs() {
    if (!modelChanged) return;

    glm::mat4 model(1.0f);

    model = glm::translate(model, getPos());

    model = glm::rotate(model, glm::radians(getRot().x), glm::vec3(1,0,0));
    model = glm::rotate(model, glm::radians(getRot().y), glm::vec3(0,1,0));
    model = glm::rotate(model, glm::radians(getRot().z), glm::vec3(0,0,1));

    model = glm::scale(model, getScl());

    auto it = buffs.find(DEFAULT_MODEL_NAME);
    if (it != buffs.end() && it->second)
        it->second->write(0, sizeof(glm::mat4), &model);

    modelChanged = false;
}

FloatCamera2d::FloatCamera2d() {
    setupObject(CameraStruct({getView(), getProj()}));
}

glm::mat4 FloatCamera2d::getProj() const {
    const glm::vec2 &res = Rendering::Window::res;
    return glm::ortho(0.0f, res.x, 0.0f, res.y);
}
glm::mat4 FloatCamera2d::getView() const {
    glm::mat4 model(1.0f);
    model = glm::translate(model, -getPos());
    model = glm::rotate(model, getRot().x, {1,0,0});
    model = glm::rotate(model, getRot().y, {0,1,0});
    model = glm::rotate(model, getRot().z, {0,0,1});
    return model;
}



FloatCamera3d::FloatCamera3d(glm::vec3 pos, glm::vec3 rot) {
    this->pos = pos;
    this->rot = rot;
    setupObject(CameraStruct({getView(), getProj()}));
}

glm::vec3 FloatCamera3d::getForwardVector() const {
    float yawRad = glm::radians(getRot().y);
    float pitchRad = glm::radians(getRot().x);
    return glm::normalize(glm::vec3(
        cos(pitchRad) * sin(yawRad),
        -sin(pitchRad),
        cos(pitchRad) * cos(yawRad)));
}
glm::vec3 FloatCamera3d::getRightVector() const {
    float yawRad = glm::radians(getRot().y);
    return glm::normalize(glm::vec3(
        -cos(yawRad),
        0.0f,
        sin(yawRad)));
}
glm::vec3 FloatCamera3d::getDownVector() const {
    glm::vec3 forward = getForwardVector();
    glm::vec3 right = getRightVector();
    return glm::normalize(glm::cross(right, forward));
}

glm::mat4 FloatCamera3d::getProj() const {
    const glm::vec2 &res = Rendering::Window::res;
    if (res.x == 0 || res.y == 0) {
        spdlog::error("Invalid resolution set");
        return glm::mat4(1.0f);
    }
    return glm::perspective(glm::radians(fov), res.x / res.y, nearPlane, farPlane);
}
glm::mat4 FloatCamera3d::getView() const {
    glm::mat4 T = glm::translate(glm::mat4(1.0f), -pos);

    glm::mat4 R =
        glm::rotate(glm::mat4(1.0f), glm::radians(-rot.x), glm::vec3(1,0,0)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(-rot.y), glm::vec3(0,1,0)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(-rot.z), glm::vec3(0,0,1));

    return R * T;
}