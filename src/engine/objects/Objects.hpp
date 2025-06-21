#pragma once
#include <glm/gtx/quaternion.hpp>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>
#include <memory>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>

#include "../customMath/BigVec.hpp"
#include "../HelperFunctions.hpp"
#include "../opengl/HelperFunctionsOpengl.hpp"

enum class CullPriority
{
    Max,
    High,
    Medium,
    Low
};

class BaseObject
{
public:
    BigVec3 position;
    glm::vec3 rotation;

    BaseObject() : position(BigVec3()) {}
    BaseObject(BigVec3 pos, glm::vec3 rot) : position(pos), rotation(rot) {}
    ~BaseObject();

    // makes the other object the parent to the current object
    void makeParent(BaseObject &other);
    // gets the exact position in the world instead of relative
    BigVec3 getTruePos();

protected:
    BaseObject *parent = nullptr;
    std::vector<BaseObject *> children;

private:
    void removeParent();
};

class Camera : public BaseObject
{
public:
    const glm::vec2 RES;
    // the two clip distances
    float near = 0.00001f;
    float far = 100000.0f;
    float fov = 90.0f; // if this number isn't 90 then you got a small wiener (or your zooming in which is chill but only if your zooming in)
    Camera(const glm::vec2 &RES, BigVec3 pos = BigVec3(), glm::vec3 rot = glm::vec3()) : BaseObject(pos, rot), RES(RES) {}
    glm::mat4 getViewMatrix() const;
    // do the thing with the projection, but like pass in near and far
    glm::mat4 getProjectionMatrix() const;
    // it gets the vector right in front
    glm::vec3 getForwardVector() const;
    // it gets the vector to the right
    glm::vec3 getRightVector() const;
    // it gets the vector down
    glm::vec3 getDownVector() const;
    // converts the regular position into the local position relative to the camera
    BigVec3 convertToLocal(const BigVec3 &otherPosition) const;

private:
    // it gets all them rotation craziness
    glm::mat4 getRotationMatrix() const;
};

class RenderObject : public BaseObject
{
public:
    RenderObject(Shader *shady, Shader *slimShady, Image *im, Camera *cam, BigVec3 pos = BigVec3(0), glm::vec3 rot = glm::vec3(0.0f), glm::vec3 scl = glm::vec3(1.0f));
    ~RenderObject();

    void Update(const float &deltaTime);
    void Draw();

    BigVec3 scale;

    static float gamma;
    static bool disableBrightness;
    static Mesh *defaultMeshAPI;

    const int NUM_VECTOR_NUMBERS = 8; // the number of stupid numbers in the vector per vert, it is x, y, z, tex1, tex2, norm x, norm y, norm z
    const Bigint bigObjectThresholdSize = Bigint(10000);

    static void UpdateAllObjects(const float &deltaTime);
    static void DrawAllObjects();
    static void updateTime();

protected:
    Mesh *mesh;
    Mesh *pointMesh;
    virtual void appendUpdate(const float &deltaTime);
    virtual void appendCustomShaderValues();
    BigVec3 tempLocalPosition;
    std::vector<float> vertices;
    const std::vector<short> vertLogic = {3, 2, 3};
    const std::vector<float> pointVert = {0, 0, 0};
    CullPriority cullPriority = CullPriority::Medium;

    const static Bigint maxDistanceMediumSquared;
    const static Bigint maxDistanceLowSquared;
    const static Bigint maxDistanceHighSquared;

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
    static std::vector<RenderObject *> renderObjects;

    Uint64 startTimeCulled = -1;
    Uint64 lastCullCheck = -1;

    const static Bigint near;
    const static Bigint far;

    static Uint64 now;
};

class PhysicsObject : public RenderObject
{
public:
    PhysicsObject(Shader *shady, Shader *slimShady, Image *im, Camera *cam, BigVec3 pos = BigVec3(0.0f), glm::vec3 rot = glm::vec3(0.0f), glm::vec3 scl = glm::vec3(1.0f));
    ~PhysicsObject();
};