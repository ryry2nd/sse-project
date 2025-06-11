#include "RenderObject.h"
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>
#include <memory>
#include "Mesh.hpp"
#include "HelperFunctions.hpp"
#include "opengl/OpenGlMesh.hpp"
#include "Camera.hpp"
#include "customMath/BigVec.hpp"
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>

class PhysicsObject : public RenderObject
{
public:
    PhysicsObject(Shader *shady, Shader *slimShady, Image *im, Camera *cam, glm::vec3 emissionColor = glm::vec3(0, 0, 0), Bigint emissionIntensity = Bigint(), BigVec3 pos = BigVec3(0.0f), glm::vec3 rot = glm::vec3(0.0f), glm::vec3 scl = glm::vec3(1.0f));
    ~PhysicsObject();
};