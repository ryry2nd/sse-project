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
    static void init(Shader *pointShader, Mesh *defaultMeshAPI, Camera *camera, float gamma = 2.5f, bool disableBrightness = false);
    RenderObject() {}
    RenderObject(Shader *shady, Image *im);
    ~RenderObject();

    static void UpdateAllObjects(const float &deltaTime);
    static void DrawAllObjects();
    static void updateTime();

protected:
    // the list of meshes
    std::vector<Mesh *> meshes;

    void setupObject(Shader *shady, Image *im);
    virtual void appendUpdate(const float &deltaTime);
    virtual void appendCustomShaderValues();
    BigVec3 tempLocalPosition;
    CullPriority cullPriority = CullPriority::Medium;

    const static std::vector<float> cubeMesh;
    static Mesh *defaultMeshAPI;

    const static Bigint maxDistanceMediumSquared;
    const static Bigint maxDistanceLowSquared;
    const static Bigint maxDistanceHighSquared;

    bool culled = false;

    void Update(const float &deltaTime);
    void Draw();

    static std::vector<float> makeTexturedCube(float size = 1.0f);
    static void addFace(std::vector<float> &verts,
                        glm::vec3 vert0,
                        glm::vec3 vert1,
                        glm::vec3 vert2,
                        glm::vec3 vert3,
                        glm::vec2 uvMin,
                        glm::vec2 uvMax);

private:
    Shader *shader;
    Image *image;
    Bigint calculateInverseSquareLaw(const BigVec3 &subtractedPos, const Bigint &intensity) const;
    Bigint calculateDistanceSquared(const BigVec3 &subtractedPos) const;
    Bigint distanceSquared;
    void renderAsPoint();
    void renderAsMesh();

    BigVec3 localSize;
    static std::vector<RenderObject *> renderObjects;

    Uint64 startTimeCulled = -1;
    Uint64 lastCullCheck = -1;

    const static Bigint near;
    const static Bigint far;

    static float gamma;
    static bool disableBrightness;

    static Uint64 now;

    static Shader *pointShader;
    static Camera *camera;
    static Mesh *pointMesh;
};

class MeshChunks : public RenderObject
{
public:
    MeshChunks(Shader *shader, Image *image)
    {
        setupObject(shader, image);

        float size = 512.0f;
        float s = 0.5f;
        glm::vec2 grid(2, 2);
        int x, y, z, i;
        float fx, fy, fz;

        std::vector<float> bigMesh;
        std::vector<float> tempVerts;
        for (x = 0; x < size; x++)
        {
            for (y = 0; y < size; y++)
            {
                for (z = 0; z < size; z++)
                {
                    tempVerts.clear();
                    fx = s * x * 2 - size;
                    fy = s * y * 2 - size;
                    fz = s * z * 2 - size;
                    // // Front face (+Z)
                    // if (z == size - 1)
                    //     addFace(tempVerts, {fx - s, fy - s, fz + s}, {fx + s, fy - s, fz + s}, {fx + s, fy + s, fz + s}, {fx - s, fy + s, fz + s}, {0.0f, 0.0f}, {1.0f, 1.0f});
                    // // Back face (-Z)
                    // if (z == 0)
                    //     addFace(tempVerts, {fx + s, fy - s, fz - s}, {fx - s, fy - s, fz - s}, {fx - s, fy + s, fz - s}, {fx + s, fy + s, fz - s}, {0.0f, 0.0f}, {1.0f, 1.0f});
                    // // Left face (-X)
                    // if (x == 0)
                    //     addFace(tempVerts, {fx - s, fy - s, fz - s}, {fx - s, fy - s, fz + s}, {fx - s, fy + s, fz + s}, {fx - s, fy + s, fz - s}, {0.0f, 0.0f}, {1.0f, 1.0f});
                    // // Right face (+X)
                    // if (x == size - 1)
                    //     addFace(tempVerts, {fx + s, fy - s, fz + s}, {fx + s, fy - s, fz - s}, {fx + s, fy + s, fz - s}, {fx + s, fy + s, fz + s}, {0.0f, 0.0f}, {1.0f, 1.0f});
                    // Top face (+Y)
                    if (y == size - 1)
                        addFace(tempVerts, {fx - s, fy + s, fz + s}, {fx + s, fy + s, fz + s}, {fx + s, fy + s, fz - s}, {fx - s, fy + s, fz - s}, {0.0f, 0.0f}, {1.0f, 1.0f});
                    // Bottom face (-Y)
                    if (y == 0)
                        addFace(tempVerts, {fx - s, fy - s, fz - s}, {fx + s, fy - s, fz - s}, {fx + s, fy - s, fz + s}, {fx - s, fy - s, fz + s}, {0.0f, 0.0f}, {1.0f, 1.0f});

                    bigMesh.insert(bigMesh.end(), tempVerts.begin(), tempVerts.end());
                }
            }
        }

        Mesh *tempMesh;
        for (x = 0; x < grid.x; x++)
        {
            for (y = 0; y < grid.y; y++)
            {
                tempMesh = defaultMeshAPI->makeNewMesh(bigMesh, {3, 2, 3});
                tempMesh->posOffset = glm::vec3(x * size, 0, y * size);
                // tempMesh->sizeOffset = glm::vec3(size);
                meshes.push_back(tempMesh);
            }
        }
    }

    void appendUpdate(const float &deltaTime) override {}
};

class PhysicsObject : public RenderObject
{
public:
    PhysicsObject(Shader *shady, Image *im);
    ~PhysicsObject();
};