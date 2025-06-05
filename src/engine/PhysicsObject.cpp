#include "PhysicsObject.h"

PhysicsObject::PhysicsObject(Shader *shady, Shader *slimShady, Image *im, Camera *cam, glm::vec3 emissionColor, Bigint emissionIntensity, BigVec3 pos, glm::vec3 rot, glm::vec3 scl)
    : RenderObject(shady, slimShady, im, cam, emissionColor, emissionIntensity, pos)
{
}

PhysicsObject::~PhysicsObject()
{
    RenderObject::~RenderObject();
}