#pragma once

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

struct Light
{
    BigVec3 &position;
    glm::vec3 color;
    Bigint intensity;
};

enum class CullPriority
{
    Max,
    High,
    Medium,
    Low
};

class RenderObject
{
public:
    RenderObject(Shader *shady, Shader *slimShady, Image *im, Camera *cam, glm::vec3 emissionColor = glm::vec3(0, 0, 0), Bigint emissionIntensity = Bigint(), BigVec3 pos = BigVec3(0.0f), glm::vec3 rot = glm::vec3(0.0f), glm::vec3 scl = glm::vec3(1.0f));
    ~RenderObject();

    void Update(const float &deltaTime);
    void Draw();

    BigVec3 position;
    glm::vec3 rotation;
    BigVec3 scale;

    BigVec3 velocity;
    BigVec3 acceleration;

    static std::vector<RenderObject *> renderObjects;

    static float gamma;
    static bool disableBrightness;
    static Mesh *defaultMeshAPI;

    const int NUM_VECTOR_NUMBERS = 8; // the number of stupid numbers in the vector per vert, it is x, y, z, tex1, tex2, normx, normy, normz
    const Bigint bigObjectThresholdSize = Bigint(10000);

    static void UpdateAllObjects(const float &deltaTime);
    static void DrawAllObjects();
    static void addStaticLight(Light *light);
    static void removeStaticLight(Light *light);

    Bigint maxDistanceMediumSquared = Bigint(100000l * 100000l);
    Bigint maxDistanceLowSquared = Bigint(300l * 300l);
    Bigint maxDistanceHighSquared = Bigint("10000000000000000000000000");

protected:
    Mesh *mesh;
    Mesh *pointMesh;
    Light *thisLight = nullptr;
    virtual void appendUpdate(const float &deltaTime);
    virtual void appendCustomShaderValues();
    RenderObject *parent = nullptr;
    BigVec3 tempLocalPosition;
    static std::vector<Light *> allLights;
    std::vector<float> vertices;
    const std::vector<short> vertLogic = {3, 2, 3};
    const std::vector<float> pointVert = {0, 0, 0};
    CullPriority cullPriority = CullPriority::Medium;
    bool culled = false;

private:
    Shader *shader;
    Shader *pointShader;
    Image *image;
    Camera *camera;
    void calculateSizes();
    Bigint calculateInverseSquareLaw(const BigVec3 &subtractedPos, const Bigint &intensity) const;
    Bigint calculateDistanceSquared(const BigVec3 &subtractedPos) const;
    Bigint distanceSquared;
    void renderAsPoint(const float &mappedDepth);
    void renderAsMesh(const float &mappedDepth, const Bigint &realSize);

    void updateVerts();
    void setupObject();

    BigVec3 localSize;
};