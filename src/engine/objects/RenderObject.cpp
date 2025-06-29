#include "Objects.hpp"

static std::vector<float> vertices = {
    // Positions          // Tex Coords (U, flipped V) // Normals
    // Front face
    -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0, 0, 1,
    0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0, 0, 1,
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0, 0, 1,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0, 0, 1,

    // Back face
    0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0, 0, -1,
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0, 0, -1,
    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0, 0, -1,
    -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0, 0, -1,

    // Left face
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -1, 0, 0,
    -0.5f, -0.5f, 0.5f, 1.0f, 1.0f, -1, 0, 0,
    -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1, 0, 0,
    -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -1, 0, 0,

    // Right face
    0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1, 0, 0,
    0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1, 0, 0,
    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1, 0, 0,
    0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 1, 0, 0,

    // Top face
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0, 1, 0,
    0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0, 1, 0,
    -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0, 1, 0,
    0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0, 1, 0,

    // Bottom face
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0, -1, 0,
    0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0, -1, 0,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0, -1, 0,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0, -1, 0};

static std::vector<unsigned int> indices = {
    // Front face
    0, 1, 2,
    1, 3, 2,

    // Back face
    4, 5, 6,
    5, 7, 6,

    // Left face
    8, 9, 10,
    9, 11, 10,

    // Right face
    12, 13, 14,
    13, 15, 14,

    // Top face
    16, 17, 18,
    17, 19, 18,

    // Bottom face
    20, 21, 22,
    21, 23, 22};

std::vector<float> RenderObject::cubeMesh = vertices;
std::vector<unsigned int> RenderObject::cubeIndices = indices;
float RenderObject::gamma = 1.0f;
bool RenderObject::disableBrightness = false;
std::vector<RenderObject *> RenderObject::renderObjects;
Uint64 RenderObject::now = SDL_GetTicks();
Mesh *RenderObject::pointMesh = nullptr;
Shader *RenderObject::pointShader = nullptr;
Camera *RenderObject::camera = nullptr;
const Bigint RenderObject::near = Bigint(0.1);
const Bigint RenderObject::far = Bigint("1000000000000000000000000000");

const Bigint RenderObject::maxDistanceMediumSquared = Bigint(100000l * 100000l);
const Bigint RenderObject::maxDistanceLowSquared = Bigint(300l * 300l);
const Bigint RenderObject::maxDistanceHighSquared = Bigint("10000000000000000000000000");

Mesh *RenderObject::defaultMeshAPI = nullptr;

void RenderObject::init(Shader *pointShader, Mesh *defaultMeshAPI, Camera *camera, float gamma, bool disableBrightness)
{
    RenderObject::pointShader = pointShader;
    RenderObject::camera = camera;
    RenderObject::defaultMeshAPI = defaultMeshAPI;
    RenderObject::pointMesh = defaultMeshAPI->makeNewMesh({0, 0, 0}, {0}, {3}, MeshTypes::Points);
    RenderObject::gamma = gamma;
    RenderObject::disableBrightness = disableBrightness;
}

RenderObject::RenderObject(Shader *shady, Image *im)
{
    setupObject(shady, im);
    meshes.push_back(defaultMeshAPI->makeNewMesh(cubeMesh, cubeIndices, {3, 2, 3}));
}

void RenderObject::setupObject(Shader *shady, Image *im)
{
    shader = shady;
    image = im;
    if (pointShader == nullptr || camera == nullptr || defaultMeshAPI == nullptr)
    {
        throw std::runtime_error("You have to run RenderObject::init(pointShader, meshApi, camera) to setup objects");
    }
    renderObjects.push_back(this);
}

RenderObject::~RenderObject()
{
    renderObjects.erase(std::find(renderObjects.begin(), renderObjects.end(), this));
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
    if (culled)
    {
        return;
    }

    appendUpdate(deltaTime);
}

Bigint RenderObject::calculateInverseSquareLaw(const BigVec3 &subtractedPos, const Bigint &intensity) const
{
    if (subtractedPos.x == 0 && subtractedPos.y == 0 && subtractedPos.z == 0)
        return Bigint(1);
    return intensity / calculateDistanceSquared(subtractedPos);
}

Bigint RenderObject::calculateDistanceSquared(const BigVec3 &subtractedPos) const
{
    return subtractedPos.x * subtractedPos.x +
           subtractedPos.y * subtractedPos.y +
           subtractedPos.z * subtractedPos.z;
}

void RenderObject::appendCustomShaderValues() {}

void RenderObject::updateTime()
{
    now = SDL_GetTicks64();
}

void RenderObject::renderAsPoint()
{
    shader->disableCulling();
    Bigint distance = distanceSquared.sqrt();
    glm::vec3 newPos = (tempLocalPosition / distance).toFloatVec3() * 10.0f;

    glm::mat4 matrix = glm::mat4(1.0f);

    float mappedDepth = glm::clamp(((distance - near) / (Bigint(100000) - near)).toFloat(), 0.0f, 0.9f);

    matrix = glm::translate(matrix, newPos);

    pointShader->includeShader();
    pointShader->setUniform("depth", mappedDepth);
    pointShader->setUniform("uModel", matrix);
    pointShader->setUniform("uView", camera->getViewMatrix());
    pointShader->setUniform("uProjection", camera->getProjectionMatrix());
    pointShader->setUniform("gamma", gamma);
    pointShader->setUniform("color", glm::vec3(1.0f, 1.0f, 1.0f));
    pointShader->setUniform("pointSize", 10.0f);
    pointMesh->Draw();
}

void RenderObject::renderAsMesh()
{
    shader->enableCulling();
    shader->includeShader();
    shader->setUniform("gamma", gamma);
    shader->setUniform("u_fullBright", disableBrightness);
    shader->setUniform("uView", camera->getViewMatrix());
    shader->setUniform("uProjection", camera->getProjectionMatrix());
    shader->setUniform("texture1", image);

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
        shader->setUniform("uModel", uModel);
        shader->setUniform("depth", mappedDepth);
        appendCustomShaderValues();
        mesh->Draw();
    }
}

void RenderObject::Draw()
{
    if (culled && (now - lastCullCheck) > 1)
    {
        lastCullCheck = now;
        return;
    }

    tempLocalPosition = camera->convertToLocal(position);
    distanceSquared = calculateDistanceSquared(tempLocalPosition);

    if (cullPriority == CullPriority::Medium && distanceSquared > maxDistanceMediumSquared)
    {
        culled = true;
        startTimeCulled = now;
        lastCullCheck = now;
        return;
    }

    if (cullPriority == CullPriority::Low && distanceSquared > maxDistanceLowSquared)
    {
        culled = true;
        startTimeCulled = now;
        lastCullCheck = now;
        return;
    }

    if (cullPriority == CullPriority::High && distanceSquared > maxDistanceHighSquared)
    {
        culled = true;
        startTimeCulled = now;
        lastCullCheck = now;
        return;
    }

    if (distanceSquared < Bigint("100000000000000000000000"))
    {
        culled = false;
        renderAsMesh();
    }
    else if (cullPriority == CullPriority::High)
    {
        culled = false;
        renderAsPoint();
    }
    else
    {
        culled = true;
        startTimeCulled = now;
        lastCullCheck = now;
    }
}

void RenderObject::UpdateAllObjects(const float &deltaTime)
{
    for (size_t i = 0; i < renderObjects.size(); i++)
    {
        renderObjects[i]->Update(deltaTime);
    }
}

void RenderObject::DrawAllObjects()
{
    for (size_t i = 0; i < renderObjects.size(); i++)
    {
        renderObjects[i]->Draw();
    }
}