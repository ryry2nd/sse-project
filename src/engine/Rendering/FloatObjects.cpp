#include "FloatObjects.hpp"
#include "Rendering.hpp"

using namespace Objects;

#define DEFAULT_MODEL_NAME "Model"

void FloatObject::setupObject() {
    buffs[DEFAULT_MODEL_NAME] = Rendering::CreationFunctions::createBuff(Rendering::Buff::Type::Uniform, Rendering::Buff::Frequency::Dynamic, sizeof(glm::mat4));
}

FloatObject::FloatObject() {
    setupObject();
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
void FloatObject::setScl(glm::vec3 scl) {
    FloatParticle::setScl(scl);
    modelChanged = true;
}

void FloatObject::updateBuffs() {
    if (modelChanged) {
        glm::mat4 model(1.0f);
        model = glm::translate(model, getPos());
        model = glm::rotate(model, getRot().x, {1,0,0});
        model = glm::rotate(model, getRot().y, {0,1,0});
        model = glm::rotate(model, getRot().z, {0,0,1});
        model = glm::scale(model, getScl());
        buffs[DEFAULT_MODEL_NAME]->write(0, sizeof(glm::mat4), &model);
        modelChanged = false;
    }
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
    model = glm::translate(model, getPos());
    model = glm::rotate(model, getRot().x, {1,0,0});
    model = glm::rotate(model, getRot().y, {0,1,0});
    model = glm::rotate(model, getRot().z, {0,0,1});
    model = glm::scale(model, getScl());
    return model;
}



FloatCamera3d::FloatCamera3d() {
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
    return glm::perspective(glm::radians(fov), res.x / res.y, nearPlane, farPlane);
}
glm::mat4 FloatCamera3d::getView() const {
    glm::mat4 model(1.0f);
    model = glm::translate(model, getPos());
    model = glm::rotate(model, getRot().x, {1,0,0});
    model = glm::rotate(model, getRot().y, {0,1,0});
    model = glm::rotate(model, getRot().z, {0,0,1});
    model = glm::scale(model, getScl());
    return model;
}