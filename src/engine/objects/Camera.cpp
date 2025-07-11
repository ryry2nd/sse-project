#include "Objects.hpp"

#include <glm/gtc/matrix_transform.hpp>

using namespace Objects;

glm::mat4 Camera::getViewMatrix() const
{
    return glm::inverse(getRotationMatrix()); // camera is at origin; only rotation matters
}

glm::mat4 Camera::getProjectionMatrix() const
{
    const glm::vec2 &res = Rendering::HelperFunctions::res;
    return glm::perspective(glm::radians(fov), res.x / res.y, near, far);
}

glm::mat4 Camera::getProjectionMatrix2d() const
{
    const glm::vec2 &res = Rendering::HelperFunctions::res;
    return glm::ortho(0.0f, res.x, 0.0f, res.y);
}

glm::vec3 Camera::getForwardVector() const
{
    float yawRad = glm::radians(rotation.y);
    float pitchRad = glm::radians(rotation.x);
    return glm::normalize(glm::vec3(
        cos(pitchRad) * sin(yawRad),
        -sin(pitchRad),
        cos(pitchRad) * cos(yawRad)));
}

glm::vec3 Camera::getRightVector() const
{
    float yawRad = glm::radians(rotation.y);
    return glm::normalize(glm::vec3(
        -cos(yawRad),
        0.0f,
        sin(yawRad)));
}

glm::vec3 Camera::getDownVector() const
{
    glm::vec3 forward = getForwardVector();
    glm::vec3 right = getRightVector();
    return glm::normalize(glm::cross(right, forward));
}

BigVec3 Camera::convertToLocal(const BigVec3 &otherPosition) const
{
    return otherPosition - position;
}

glm::mat4 Camera::getRotationMatrix() const
{
    glm::mat4 pitchMat = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1, 0, 0));
    glm::mat4 yawMat = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0, 1, 0));
    glm::mat4 rollMat = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1));
    return yawMat * pitchMat * rollMat;
}

void Camera::rotateCamera(glm::vec2 motion)
{
    const float &deltaTime = Rendering::HelperFunctions::deltaTime;
    if (deltaTime == 0)
    {
        return;
    }
    rotation.y -= (motion.x / deltaTime) * mouse_sensitivity * deltaTime;
    rotation.x -= (motion.y / deltaTime) * mouse_sensitivity * deltaTime;
}