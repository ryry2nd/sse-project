#include "Objects.hpp"

void addFace(std::vector<float> &verts,
             glm::vec3 vert0,
             glm::vec3 vert1,
             glm::vec3 vert2,
             glm::vec3 vert3,
             glm::vec2 uvMin,
             glm::vec2 uvMax)
{
    glm::vec2 uv0 = {uvMin.x, uvMax.y}; // bottom-left
    glm::vec2 uv1 = {uvMax.x, uvMax.y}; // bottom-right
    glm::vec2 uv2 = {uvMax.x, uvMin.y}; // top-right
    glm::vec2 uv3 = {uvMin.x, uvMin.y}; // top-left

    // normals
    glm::vec3 edge1 = vert1 - vert0;
    glm::vec3 edge2 = vert2 - vert0;
    glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

    auto add = [&](glm::vec3 pos, glm::vec2 uv)
    {
        verts.insert(verts.end(), {pos.x, pos.y, pos.z,
                                   uv.x, uv.y,
                                   normal.x, normal.y, normal.z});
    };

    // Triangle 1
    add(vert0, uv0);
    add(vert1, uv1);
    add(vert2, uv2);

    // Triangle 2
    add(vert2, uv2);
    add(vert3, uv3);
    add(vert0, uv0);
}

std::vector<float> makeTexturedCube(float size = 1.0f)
{
    std::vector<float> verts;
    float s = size / 2.0f;

    // Front face (+Z)
    addFace(verts, {-s, -s, s}, {s, -s, s}, {s, s, s}, {-s, s, s}, {0.0f, 0.0f}, {1.0f, 1.0f});
    // Back face (-Z)
    addFace(verts, {s, -s, -s}, {-s, -s, -s}, {-s, s, -s}, {s, s, -s}, {0.0f, 0.0f}, {1.0f, 1.0f});
    // Left face (-X)
    addFace(verts, {-s, -s, -s}, {-s, -s, s}, {-s, s, s}, {-s, s, -s}, {0.0f, 0.0f}, {1.0f, 1.0f});
    // Right face (+X)
    addFace(verts, {s, -s, s}, {s, -s, -s}, {s, s, -s}, {s, s, s}, {0.0f, 0.0f}, {1.0f, 1.0f});
    // Top face (+Y)
    addFace(verts, {-s, s, s}, {s, s, s}, {s, s, -s}, {-s, s, -s}, {0.0f, 0.0f}, {1.0f, 1.0f});
    // Bottom face (-Y)
    addFace(verts, {-s, -s, -s}, {s, -s, -s}, {s, -s, s}, {-s, -s, s}, {0.0f, 0.0f}, {1.0f, 1.0f});

    return verts;
}

float RenderObject::gamma = 2.5f;
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

void RenderObject::init(Shader *pointShader, Mesh *defaultMeshAPI, Camera *camera)
{
    RenderObject::pointShader = pointShader;
    RenderObject::camera = camera;
    RenderObject::defaultMeshAPI = defaultMeshAPI;
    RenderObject::pointMesh = defaultMeshAPI->makeNewMesh({0, 0, 0}, {3}, MeshTypes::Points);
}

RenderObject::RenderObject(Shader *shady, Image *im, const BigVec3 &pos, const glm::vec3 &rot, const glm::vec3 &scl)
{
    setupObject(shady, im, pos, rot);
    meshes.push_back(defaultMeshAPI->makeNewMesh(makeTexturedCube(), {3, 2, 3}));
    meshes.back()->sizeOffset = scl;
}

void RenderObject::setupObject(Shader *shady, Image *im, const BigVec3 &pos, const glm::vec3 &rot)
{
    position = pos;
    rotation = rot;
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

void RenderObject::renderAsMesh(Mesh *mesh, glm::mat4 matrix, const float &transform, float mappedDepth)
{
    if (transform != 0)
    {
        matrix = glm::translate(matrix, mesh->posOffset / transform);
        matrix = glm::scale(matrix, mesh->sizeOffset / transform);
    }

    shader->setUniform("uModel", matrix);
    shader->setUniform("depth", mappedDepth);
    appendCustomShaderValues();
    mesh->Draw();
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
        shader->includeShader();
        shader->setUniform("gamma", gamma);
        shader->setUniform("u_fullBright", disableBrightness);
        shader->setUniform("uView", camera->getViewMatrix());
        shader->setUniform("uProjection", camera->getProjectionMatrix());
        shader->setUniform("texture1", image);

        glm::vec3 newPos;
        // glm::vec3 newSize;

        Bigint distance = distanceSquared.sqrt();

        Bigint transform;

        if (distance > Bigint(10000))
        {
            transform = distance * Bigint(0.1f) + Bigint(0.1f);
            newPos = (tempLocalPosition / transform).toFloatVec3();
            // newSize = (BigVec3(mesh->sizeOffset) / transform).toFloatVec3();
        }
        else
        {
            newPos = tempLocalPosition.toFloatVec3();
            // newSize = mesh->sizeOffset;
            transform = Bigint(1);
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

        float mappedDepth;

        if (distance < Bigint(10000))
        {
            mappedDepth = glm::clamp(((distance - near) / (Bigint(10000) - near)).toFloat() * 0.5f, 0.0f, 0.5f);
        }
        else if (distance < Bigint(10000000000))
        {
            mappedDepth = glm::clamp((((distance - Bigint(10000)) / (Bigint(10000000000) - Bigint(10000))).toFloat() * 0.2f) + 0.5f, 0.5f, 0.9f);
        }
        else
        {
            mappedDepth = glm::clamp((((distance - Bigint(10000000000)) / (far - Bigint(10000000000))).toFloat() * 0.3f) + 0.7f, 0.7f, 0.999999f);
        }

        for (Mesh *mesh : meshes)
            renderAsMesh(mesh, matrix, passedTransform, mappedDepth);
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