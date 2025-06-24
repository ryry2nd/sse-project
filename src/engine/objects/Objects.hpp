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

    BaseObject() : position(BigVec3()), rotation(glm::vec3()) {}
    BaseObject(const BigVec3 &pos, const glm::vec3 &rot) : position(pos), rotation(rot) {}
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
    // run before you setup any object
    static void init(Shader *pointShader, Mesh *defaultMeshAPI, Camera *camera);
    RenderObject(Shader *shady, Image *im, const BigVec3 &pos = BigVec3(), const glm::vec3 &rot = glm::vec3(), const glm::vec3 &scl = glm::vec3(1.0f));
    ~RenderObject();

    void Update(const float &deltaTime);
    void Draw();

    static float gamma;
    static bool disableBrightness;

    static void UpdateAllObjects(const float &deltaTime);
    static void DrawAllObjects();
    static void updateTime();

protected:
    // the list of meshes
    std::vector<Mesh *> meshes;

    void setupObject(Shader *shady, Image *im, const BigVec3 &pos = BigVec3(), const glm::vec3 &rot = glm::vec3());
    virtual void appendUpdate(const float &deltaTime);
    virtual void appendCustomShaderValues();
    BigVec3 tempLocalPosition;
    CullPriority cullPriority = CullPriority::Medium;

    const static Bigint maxDistanceMediumSquared;
    const static Bigint maxDistanceLowSquared;
    const static Bigint maxDistanceHighSquared;

    bool culled = false;

private:
    Shader *shader;
    Image *image;
    Bigint calculateInverseSquareLaw(const BigVec3 &subtractedPos, const Bigint &intensity) const;
    Bigint calculateDistanceSquared(const BigVec3 &subtractedPos) const;
    Bigint distanceSquared;
    void renderAsPoint();
    void renderAsMesh();

    void setupObject();

    BigVec3 localSize;
    static std::vector<RenderObject *> renderObjects;

    Uint64 startTimeCulled = -1;
    Uint64 lastCullCheck = -1;

    const static Bigint near;
    const static Bigint far;

    static Uint64 now;

    static Shader *pointShader;
    static Camera *camera;
    static Mesh *pointMesh;
    static Mesh *defaultMeshAPI;
};

class PhysicsObject : public RenderObject
{
public:
    PhysicsObject(Shader *shady, Image *im, const BigVec3 &pos = BigVec3(), const glm::vec3 &rot = glm::vec3(), const glm::vec3 &scl = glm::vec3(1.0f));
    ~PhysicsObject();
};