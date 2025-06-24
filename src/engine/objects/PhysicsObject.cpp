#include "Objects.hpp"

PhysicsObject::PhysicsObject(Shader *shady, Image *im, const BigVec3 &pos, const glm::vec3 &rot, const glm::vec3 &scl)
    : RenderObject(shady, im, pos, rot, scl)
{
}

PhysicsObject::~PhysicsObject()
{
}