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
Mesh *RenderObject::defaultMeshAPI = new OpenGlMesh();
Uint64 RenderObject::now = SDL_GetTicks();
const Bigint RenderObject::near = Bigint(0.1);
const Bigint RenderObject::far = Bigint("1000000000000000000000000000");

RenderObject::RenderObject(Shader *shady, Shader *slimShady, Image *im, Camera *cam, BigVec3 pos, glm::vec3 rot, glm::vec3 scl)
    : position(pos), rotation(rot), scale(scl), shader(shady), pointShader(slimShady), image(im), camera(cam), velocity(BigVec3()), acceleration(BigVec3())
{
    mesh = defaultMeshAPI->makeNewMesh();
    pointMesh = defaultMeshAPI->makeNewMesh();

    renderObjects.push_back(this);

    vertices = makeTexturedCube();
    setupObject();
}

RenderObject::~RenderObject()
{
    renderObjects.erase(std::find(renderObjects.begin(), renderObjects.end(), this));
    delete mesh;
    delete pointMesh;
}

void RenderObject::setupObject()
{
    calculateSizes();

    mesh->setupObject(vertices, vertLogic);
    pointMesh->setupObject(pointVert, {3}, MeshTypes::Points);
}

void RenderObject::calculateSizes()
{
    glm::vec3 minCorner(FLT_MAX);
    glm::vec3 maxCorner(-FLT_MAX);

    for (size_t i = 0; i < vertices.size(); i += NUM_VECTOR_NUMBERS)
    {
        glm::vec3 pos(vertices[i], vertices[i + 1], vertices[i + 2]);
        minCorner = glm::min(minCorner, pos);
        maxCorner = glm::max(maxCorner, pos);
    }

    localSize = BigVec3(maxCorner - minCorner);
}

void RenderObject::updateVerts()
{
    // calculateSizes();
    // mesh->updateVerts(vertices);
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

    if (!velocity.isZero())
    {
        position += velocity * deltaTime;
    }
    if (!acceleration.isZero())
    {
        velocity += acceleration * deltaTime;
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

void RenderObject::renderAsPoint(const float &mappedDepth)
{
    glm::vec3 newPos = (tempLocalPosition / distanceSquared.sqrt()).toFloatVec3() * 10.0f;

    glm::mat4 matrix = glm::mat4(1.0f);

    matrix = glm::translate(matrix, newPos);

    pointShader->includeShader();
    pointShader->setUniform("depth", mappedDepth);
    pointShader->setUniform("uModel", matrix);
    pointShader->setUniform("uView", camera->getViewMatrix());
    pointShader->setUniform("uProjection", camera->getProjectionMatrix());
    pointShader->setUniform("gamma", gamma);
    pointShader->setUniform("color", glm::vec3(1.0f, 1.0f, 1.0f));
    pointShader->setUniform("pointSize", 10.0f);
    pointMesh->finalizeShaders();
}

void RenderObject::renderAsMesh(const float &mappedDepth, const Bigint &realSize)
{
    glm::vec3 newPos;
    glm::vec3 newSize;

    if (realSize > bigObjectThresholdSize)
    {
        Bigint transform = realSize / Bigint(10);

        newPos = (tempLocalPosition / transform).toFloatVec3();
        newSize = (scale / transform).toFloatVec3();
    }
    else
    {
        newPos = tempLocalPosition.toFloatVec3();
        newSize = scale.toFloatVec3();
    }

    glm::mat4 matrix = glm::mat4(1.0f);

    // converts the position to be local to the camera
    matrix = glm::translate(matrix, newPos);

    // rotates the model
    matrix = glm::rotate(matrix, rotation.x, glm::vec3(1, 0, 0));
    matrix = glm::rotate(matrix, rotation.y, glm::vec3(0, 1, 0));
    matrix = glm::rotate(matrix, rotation.z, glm::vec3(0, 0, 1));

    // scales the model
    matrix = glm::scale(matrix, newSize);

    shader->includeShader();
    shader->setUniform("uModel", matrix);
    shader->setUniform("depth", mappedDepth);
    shader->setUniform("uView", camera->getViewMatrix());
    shader->setUniform("uProjection", camera->getProjectionMatrix());
    shader->setUniform("gamma", gamma);
    shader->setUniform("u_fullBright", disableBrightness);

    appendCustomShaderValues();
    shader->setUniform("texture1", image);
    mesh->finalizeShaders();
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

    Bigint realSize = (localSize * scale).getMaxAbs();

    float mappedDepth;

    if (realSize < Bigint(100000))
    {
        mappedDepth = glm::clamp(((distanceSquared.sqrt() - near) / (Bigint(100000) - near)).toFloat() * 0.5f, 0.0f, 0.5f);
    }
    else if (realSize < Bigint(10000000000))
    {
        mappedDepth = glm::clamp((((distanceSquared.sqrt() - Bigint(100000)) / (Bigint(10000000000) - Bigint(100000))).toFloat() * 0.2f) + 0.5f, 0.5f, 0.9f);
    }
    else
    {
        mappedDepth = glm::clamp((((distanceSquared.sqrt() - Bigint(10000000000)) / (far - Bigint(10000000000))).toFloat() * 0.3f) + 0.7f, 0.7f, 0.999999f);
    }

    if (distanceSquared / (realSize * realSize) < Bigint("30000"))
    {
        culled = false;
        renderAsMesh(mappedDepth, realSize);
    }
    else if (cullPriority == CullPriority::High)
    {
        culled = false;
        renderAsPoint(mappedDepth);
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