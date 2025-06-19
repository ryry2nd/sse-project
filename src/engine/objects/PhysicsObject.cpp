#include "Objects.hpp"

PhysicsObject::PhysicsObject(Shader *shady, Shader *slimShady, Image *im, Camera *cam, BigVec3 pos, glm::vec3 rot, glm::vec3 scl)
    : RenderObject(shady, slimShady, im, cam, pos, rot, scl)
{
}

PhysicsObject::~PhysicsObject()
{
}