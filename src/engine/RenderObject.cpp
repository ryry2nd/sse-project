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

RenderObject::RenderObject(Backend *backend, Shader *shady, Image *im, Camera *cam, glm::vec3 emissionColor, Bigint emissionIntensity, BigVec3 pos, glm::vec3 rot, glm::vec3 scl)
    : position(pos),
      rotation(rot), scale(scl), shader(shady), image(im), camera(cam), velocity(BigVec3(Bigint(), Bigint(), Bigint())), acceleration(BigVec3(Bigint(), Bigint(), Bigint()))
{
    this->backend = backend;
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
    delete backend;
    if (thisLight != nullptr)
    {
        allLights.erase(std::find(allLights.begin(), allLights.end(), thisLight));
        renderObjects.erase(std::find(renderObjects.begin(), renderObjects.end(), this));
        delete thisLight;
    }
}

void RenderObject::setupObject()
{
    calculateSizes();

    backend->setupObject(vertices);
}

void RenderObject::calculateSizes()
{
    minCorner = glm::vec3(FLT_MAX);
    maxCorner = glm::vec3(-FLT_MAX);

    for (size_t i = 0; i < vertices.size(); i += NUM_VECTOR_NUMBERS)
    {
        glm::vec3 pos(vertices[i], vertices[i + 1], vertices[i + 2]);
        minCorner = glm::min(minCorner, pos);
        maxCorner = glm::max(maxCorner, pos);
    }

    localSize = maxCorner - minCorner;
}

void RenderObject::updateVerts()
{
    calculateSizes();
    backend->updateVerts(vertices);
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

// it culls everything close and its different depending on the near value
float RenderObject::nearCullFunction() const
{
    return near <= 0.1f ? 0.0f : 100.0f;
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

void RenderObject::addVarsToShader()
{
    Bigint realSize = (BigVec3(localSize) * scale).getMaxAbs();
    glm::vec3 newPos;
    glm::vec3 newSize;

    if (realSize == 0)
    {
        return;
    }

    if (realSize > bigObjectThresholdSize) // || calculateDistanceSquared(tempLocalPosition) > bigObjectThresholdDistanceSquared)
    {
        Bigint transform = realSize / Bigint(10);

        newPos = (tempLocalPosition / transform).toFloatVec3();
        newSize = (scale / transform).toFloatVec3();

        if (newSize.x < 0.001)
        {
            return;
        }
        std::cout << newPos.x << "\n";
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

    backend->includeMat4("uModel", matrix);
    // backend->includeFloat("uScaleFactor", transform.toFloat());
    backend->includeMat4("uView", camera->getViewMatrix());
    backend->includeMat4("uProjection", camera->getProjectionMatrix(near, far));
    backend->includeFloat("u_CullRadius", nearCullFunction());
    backend->includeFloat("gamma", gamma);
    backend->includeBool("u_fullBright", disableBrightness);

    if (thisLight != nullptr)
    {
        backend->includeTripleFloat("emissionColor", thisLight->color.x, thisLight->color.y, thisLight->color.z);
        backend->includeFloat("emissionIntensity", calculateInverseSquareLaw(tempLocalPosition, thisLight->intensity).toFloat());
    }
    else
    {
        backend->includeTripleFloat("emissionColor", 0.0f, 0.0f, 0.0f);
        backend->includeFloat("emissionIntensity", 0.0f);
    }

    int i = 0;
    glm::dvec3 temp;
    BigVec3 bigTemp;
    for (const Light *l : allLights)
    {
        if (l != thisLight)
        {
            bigTemp = l->position - position;
            temp = bigTemp.toDoubleVec3();
            if (!std::isinf(temp.x) && !std::isinf(temp.y) && !std::isinf(temp.z))
            {
                glm::dvec3 lightPos = glm::normalize(temp);
                backend->includeTripleFloat("lightPositions[" + std::to_string(i) + "]", lightPos.x, lightPos.y, lightPos.z);
                backend->includeTripleFloat("lightColors[" + std::to_string(i) + "]", l->color.x, l->color.y, l->color.z);
                backend->includeFloat("lightIntensities[" + std::to_string(i) + "]", calculateInverseSquareLaw(bigTemp, l->intensity).toFloat());
                i++;
            }
        }
    }

    backend->includeInt("numLights", i);
}

void RenderObject::Draw()
{
    tempLocalPosition = camera->convertToLocal(position);
    backend->includeShader(shader);
    addVarsToShader();
    appendCustomShaderValues();
    backend->includeTexture(image);
    backend->finalizeShaders(vertices);
}

void RenderObject::UpdateAllObjects(const float &deltaTime)
{
    for (unsigned long long i = 0; i < renderObjects.size(); i++)
    {
        renderObjects[i]->Update(deltaTime);
    }
}

void RenderObject::DrawAllObjects()
{
    for (unsigned long long i = 0; i < renderObjects.size(); i++)
    {
        renderObjects[i]->Draw();
    }
}