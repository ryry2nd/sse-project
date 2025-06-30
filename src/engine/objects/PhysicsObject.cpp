#include "Objects.hpp"

using namespace Objects;

PhysicsObject::PhysicsObject(Shader *shady, Image *im)
    : RenderObject(shady, im)
{
}

PhysicsObject::~PhysicsObject()
{
}