#pragma once
#include "customMath/BigVec.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

class Camera
{
public:
    const glm::vec2 RES;

    BigVec3 position;   // camera always at (0,0,0) but like this helps because otherwise its confusing
    float yaw = 0.0f;   // left and right
    float pitch = 0.0f; // up and down
    float roll = 0.0f;  // all sideways and stuff

    // the two clip distances
    float near = 0.01f;
    float far = 10000.0f;

    float fov = 90.0f; // if this number isn't 90 then your not a man (or your zooming in which is chill but only if your zooming in)

    Camera(const glm::vec2 &RES, Bigint x = Bigint(0), Bigint y = Bigint(0), Bigint z = Bigint(0)) : position(BigVec3(x, y, z)), RES(RES) {}

    glm::mat4 getViewMatrix() const
    {
        glm::mat4 rot = getRotationMatrix();
        return rot; // camera is at origin; only rotation matters
    }

    // do the thing with the projection, but like pass in near and far
    glm::mat4 getProjectionMatrix() const
    {
        return glm::perspective(glm::radians(fov), RES.x / RES.y, near, far);
    }

    // it gets the vector right in front
    glm::vec3 getForwardVector() const
    {
        float yawRad = glm::radians(yaw);
        float pitchRad = glm::radians(pitch);
        return glm::normalize(glm::vec3(
            cos(pitchRad) * sin(yawRad),
            -sin(pitchRad),
            cos(pitchRad) * cos(yawRad)));
    }

    // it gets the vector to the right
    glm::vec3 getRightVector() const
    {
        float yawRad = glm::radians(yaw);
        return glm::normalize(glm::vec3(
            -cos(yawRad),
            0.0f,
            sin(yawRad)));
    }

    // it gets the vector down
    glm::vec3 getDownVector() const
    {
        glm::vec3 forward = getForwardVector();
        glm::vec3 right = getRightVector();
        return glm::normalize(glm::cross(right, forward));
    }

    glm::vec3 convertToLocal(const BigVec3 &otherPosition) const
    {
        return (position - otherPosition).toFloatVec3();
    }

private:
    // it gets all them rotation craziness
    glm::mat4 getRotationMatrix() const
    {
        glm::mat4 pitchMat = glm::rotate(glm::mat4(1.0f), glm::radians(-pitch), glm::vec3(1, 0, 0));
        glm::mat4 yawMat = glm::rotate(glm::mat4(1.0f), glm::radians(-yaw), glm::vec3(0, 1, 0));
        glm::mat4 rollMat = glm::rotate(glm::mat4(1.0f), glm::radians(-roll), glm::vec3(0, 0, 1));
        return rollMat * pitchMat * yawMat;
    }
};
