#include "RenderObject.h"

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

std::vector<Light *> RenderObject::allLights;
float RenderObject::gamma = 2.5f;
bool RenderObject::disableBrightness = false;
std::vector<RenderObject *> RenderObject::renderObjects;
Mesh *RenderObject::defaultMeshAPI = new OpenGlMesh();
Uint64 RenderObject::now = SDL_GetTicks();

void RenderObject::addStaticLight(Light *light)
{
    allLights.push_back(light);
}

void RenderObject::removeStaticLight(Light *light)
{
    allLights.erase(std::find(allLights.begin(), allLights.end(), light));
}

RenderObject::RenderObject(Shader *shady, Shader *slimShady, Image *im, Camera *cam, glm::vec3 emissionColor, Bigint emissionIntensity, BigVec3 pos, glm::vec3 rot, glm::vec3 scl)
    : position(pos), rotation(rot), scale(scl), shader(shady), pointShader(slimShady), image(im), camera(cam), velocity(BigVec3(Bigint(), Bigint(), Bigint())), acceleration(BigVec3(Bigint(), Bigint(), Bigint()))
{
    mesh = defaultMeshAPI->makeNewMesh();
    pointMesh = defaultMeshAPI->makeNewMesh();

    renderObjects.push_back(this);
    if (emissionIntensity != 0.0f)
    {
        thisLight = new Light{position, emissionColor, emissionIntensity};
        allLights.push_back(thisLight);
    }

    vertices = makeTexturedCube();
    setupObject();
}

RenderObject::~RenderObject()
{
    if (thisLight != nullptr)
    {
        allLights.erase(std::find(allLights.begin(), allLights.end(), thisLight));
        delete thisLight;
    }

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
    calculateSizes();
    mesh->updateVerts(vertices);
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

    pointMesh->includeShader(pointShader);
    pointMesh->includeFloat("depth", mappedDepth);
    pointMesh->includeMat4("uModel", matrix);
    pointMesh->includeMat4("uView", camera->getViewMatrix());
    pointMesh->includeMat4("uProjection", camera->getProjectionMatrix());
    pointMesh->includeFloat("gamma", gamma);
    pointMesh->includeTripleFloat("color", 1.0f, 1.0f, 1.0f);
    pointMesh->includeFloat("pointSize", 10.0f);
    pointMesh->finalizeShaders(pointVert);
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

    mesh->includeShader(shader);
    mesh->includeMat4("uModel", matrix);
    mesh->includeFloat("depth", mappedDepth);
    mesh->includeMat4("uView", camera->getViewMatrix());
    mesh->includeMat4("uProjection", camera->getProjectionMatrix());
    // mesh->includeFloat("u_CullRadius", nearCullFunction());
    mesh->includeFloat("gamma", gamma);
    mesh->includeBool("u_fullBright", disableBrightness);

    if (thisLight != nullptr)
    {
        float intensity = calculateInverseSquareLaw(tempLocalPosition, thisLight->intensity).toFloat();
        mesh->includeTripleFloat("emissionColor", thisLight->color.x * intensity, thisLight->color.y * intensity, thisLight->color.z * intensity);
    }
    else
    {
        mesh->includeTripleFloat("emissionColor", 0.0f, 0.0f, 0.0f);
    }

    int i = 0;
    glm::dvec3 temp;
    BigVec3 bigTemp;
    glm::dvec3 lightPos;
    float intensity;
    for (const Light *l : allLights)
    {
        if (l != thisLight)
        {
            bigTemp = l->position - position;
            temp = bigTemp.toDoubleVec3();
            if (!std::isinf(temp.x) && !std::isinf(temp.y) && !std::isinf(temp.z))
            {
                lightPos = glm::normalize(temp);
                intensity = calculateInverseSquareLaw(bigTemp, l->intensity).toFloat();
                if (intensity < 0.005)
                {
                    continue;
                }
                mesh->includeTripleFloat("lightPositions[" + std::to_string(i) + "]", lightPos.x, lightPos.y, lightPos.z);
                mesh->includeTripleFloat("lightColors[" + std::to_string(i) + "]", l->color.x * intensity, l->color.y * intensity, l->color.z * intensity);
                i++;
            }
        }
    }

    mesh->includeInt("numLights", i);
    appendCustomShaderValues();
    mesh->includeTexture(image);
    mesh->finalizeShaders(vertices);
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

    BigVec3 lockedSize = localSize * scale;

    Bigint realSize = lockedSize.getMaxAbs();
    Bigint small = lockedSize.getMinAbs() / 10;

    float mappedDepth = glm::clamp(((distanceSquared.sqrt() - small) / ((realSize * Bigint(100)) - small)).toFloat(), 0.0f, 0.999999f);

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