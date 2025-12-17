#include "Objects.hpp"

// const std::vector<float> Objects::cubeVertices = {
//     // Positions          // Tex Coords (U, flipped V) // Normals
//     // Front face
//     -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0, 0, 1,
//     0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0, 0, 1,
//     -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0, 0, 1,
//     0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0, 0, 1,

//     // Back face
//     0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0, 0, -1,
//     -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0, 0, -1,
//     0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0, 0, -1,
//     -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0, 0, -1,

//     // Left face
//     -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -1, 0, 0,
//     -0.5f, -0.5f, 0.5f, 1.0f, 1.0f, -1, 0, 0,
//     -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1, 0, 0,
//     -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -1, 0, 0,

//     // Right face
//     0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1, 0, 0,
//     0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1, 0, 0,
//     0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1, 0, 0,
//     0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 1, 0, 0,

//     // Top face
//     -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0, 1, 0,
//     0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0, 1, 0,
//     -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0, 1, 0,
//     0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0, 1, 0,

//     // Bottom face
//     -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0, -1, 0,
//     0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0, -1, 0,
//     -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0, -1, 0,
//     0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0, -1, 0};

// const std::vector<unsigned int> Objects::cubeIndices = {
//     // Front face
//     0, 1, 2,
//     1, 3, 2,

//     // Back face
//     4, 5, 6,
//     5, 7, 6,

//     // Left face
//     8, 9, 10,
//     9, 11, 10,

//     // Right face
//     12, 13, 14,
//     13, 15, 14,

//     // Top face
//     16, 17, 18,
//     17, 19, 18,

//     // Bottom face
//     20, 21, 22,
//     21, 23, 22};

using namespace Objects;
using namespace Rendering;

bool RenderObject::disableBrightness = false;
const Bigint RenderObject::near = Bigint(0.1);
const Bigint RenderObject::far = Bigint("1000000000000000000000000000");

void RenderObject::init(bool disableBrightness)
{
    RenderObject::disableBrightness = disableBrightness;
}

RenderObject::RenderObject(Mesh *mesh)
{
    setupObject();
    meshes.push_back(mesh);
}

RenderObject::~RenderObject()
{
    for (Mesh *mesh : meshes)
    {
        delete mesh;
    }
}

void RenderObject::appendUpdate(const float &deltaTime)
{
    rotation.y -= deltaTime;
    rotation.x -= deltaTime;
    rotation.z -= deltaTime;
}

// I just made the default update to rotate all around
void RenderObject::Update(const float &deltaTime)
{
    appendUpdate(deltaTime);
}

Bigint RenderObject::calculateDistanceSquared(const BigVec3 &subtractedPos) const
{
    return subtractedPos.x * subtractedPos.x +
           subtractedPos.y * subtractedPos.y +
           subtractedPos.z * subtractedPos.z;
}

void RenderObject::renderMesh()
{

    glm::vec3 newPos;

    Bigint distance = distanceSquared.sqrt();

    Bigint transform;

    if (distance > Bigint(10000))
    {
        transform = distance * Bigint(0.1f) + Bigint(0.1f);
        newPos = (tempLocalPosition / transform).toFloatVec3();
    }
    else
    {
        transform = Bigint(1);
        newPos = tempLocalPosition.toFloatVec3();
    }

    glm::mat4 matrix(1.0f);
    matrix = glm::translate(matrix, newPos);
    matrix = glm::rotate(matrix, rotation.x, glm::vec3(1, 0, 0));
    matrix = glm::rotate(matrix, rotation.y, glm::vec3(0, 1, 0));
    matrix = glm::rotate(matrix, rotation.z, glm::vec3(0, 0, 1));

    float passedTransform = transform.toFloat();
    if (std::isinf(passedTransform))
    {
        passedTransform = 0;
    }

    Bigint passedNear;
    Bigint passedFar;
    float clampedNear, clampedFar;

    if (distance < Bigint(100000))
    {
        passedNear = near;
        passedFar = Bigint(100000);
        clampedNear = 0.0f;
        clampedFar = 0.5f;
    }
    else if (distance < Bigint(10000000000))
    {
        passedNear = Bigint(100000);
        passedFar = Bigint(10000000000);
        clampedNear = 0.5f;
        clampedFar = 0.7f;
    }
    else
    {
        passedNear = Bigint(10000000000);
        passedFar = far;
        clampedNear = 0.7f;
        clampedFar = 0.999999f;
    }

    float mappedDepth;
    float floatTransform = transform.toFloat();

    glm::mat4 uModel;

    bool isInf = std::isinf(floatTransform);
    bool check = floatTransform != 0 && !isInf;

    float floatPassedNear = passedNear.toFloat();
    float floatPassedFar = passedFar.toFloat();

    Shader *shader;
    for (Mesh *mesh : meshes)
    {
        if (check)
        {
            uModel = glm::translate(matrix, mesh->posOffset / floatTransform);
            uModel = glm::scale(uModel, mesh->sizeOffset / floatTransform);
        }
        else
        {
            uModel = matrix;
        }

        if (isInf)
        {
            mappedDepth = glm::clamp(((Bigint(glm::length(glm::vec3(uModel[3]))) * transform - passedNear) / (passedFar - passedNear)).toFloat() * (clampedFar - clampedNear) + clampedNear, clampedNear, clampedFar);
        }
        else
        {
            mappedDepth = glm::clamp(((glm::length(glm::vec3(uModel[3])) * floatTransform - floatPassedNear) / (floatPassedFar - floatPassedNear)) * (clampedFar - clampedNear) + clampedNear, clampedNear, clampedFar);
        }
        shader = mesh->shader;
        
        shader->SetShader();
        shader->setUniform("u_fullBright", disableBrightness);
        shader->setUniform("uView", Objects::globalCamera.getViewMatrix());
        shader->setUniform("uProjection", Objects::globalCamera.getProjectionMatrix());
        shader->setUniform("uModel", uModel);
        shader->setUniform("depth", mappedDepth);
        mesh->Draw();
    }
}

void RenderObject::Draw()
{
    tempLocalPosition = Objects::globalCamera.convertToLocal(getTruePos());
    distanceSquared = calculateDistanceSquared(tempLocalPosition);
    renderMesh();
}