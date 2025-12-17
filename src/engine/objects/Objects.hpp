#pragma once

#include "../customMath/BigVec.hpp"
#include "../../rendering/base/HelperFunctions.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>
#include <algorithm>
#include <cmath>

namespace Objects
{
    enum class CullPriority
    {
        Max,
        High,
        Medium,
        Low
    };

    struct BaseObject
    {
        BigVec3 position;
        glm::vec3 rotation;

        BaseObject() : position(BigVec3()), rotation(glm::vec3(0.0f)) {}
        BaseObject(const BigVec3 &pos, const glm::vec3 &rot) : position(pos), rotation(rot) {}
        ~BaseObject();

        // makes the other object the parent to the current object
        void makeParent(BaseObject &other);
        // gets the exact position in the world instead of relative
        BigVec3 getTruePos();
        BaseObject *parent = nullptr;
        std::vector<BaseObject *> children;
        void removeParent();
    };

    class Camera : public BaseObject
    {
    public:
        float fov = 90.0f; // if this number isn't 90 then you got a small wiener (or your zooming in which is chill but only if your zooming in)
        float mouse_sensitivity = 0.1f;
        Camera() : BaseObject() {}
        Camera(BigVec3 &pos, glm::vec3 &rot) : BaseObject(pos, rot) {}
        // takes the cameras rotation into account
        glm::mat4 getViewMatrix() const;
        // do the thing with the projection, but like pass in near and far
        glm::mat4 getProjectionMatrix() const;
        // does all of them projections but in 2d
        glm::mat4 getProjectionMatrix2d() const;
        // it gets the vector right in front
        glm::vec3 getForwardVector() const;
        // it gets the vector to the right
        glm::vec3 getRightVector() const;
        // it gets the vector down
        glm::vec3 getDownVector() const;
        // converts the regular position into the local position relative to the camera
        BigVec3 convertToLocal(const BigVec3 &otherPosition) const;
        // rotates the camera by the event motion
        void rotateCamera(glm::vec2 motion);
        // the two clip distances
        float near = 0.00001f;
        float far = 100000.0f;

    private:
        // it gets all them rotation craziness
        glm::mat4 getRotationMatrix() const;
    };

    inline Camera globalCamera;

    struct Drawable
    {
        void setupObject();
        ~Drawable();
        static void UpdateAllObjects();
        static void DrawAllObjects();

    protected:
        virtual void Draw() = 0;
        virtual void Update(const float &deltaTime) = 0;

    private:
        static std::vector<Drawable *> drawables;
    };

    class RenderObject : public Drawable, public BaseObject
    {
    public:
        // run before you setup any object
        static void init(bool disableBrightness = false);
        RenderObject() {}
        RenderObject(Rendering::Mesh *mesh);
        ~RenderObject();

    protected:
        // the list of meshes
        std::vector<Rendering::Mesh *> meshes;

        virtual void appendUpdate(const float &deltaTime);
        BigVec3 tempLocalPosition;

        void Update(const float &deltaTime) override;
        void Draw();

    private:
        Bigint calculateInverseSquareLaw(const BigVec3 &subtractedPos, const Bigint &intensity) const;
        Bigint calculateDistanceSquared(const BigVec3 &subtractedPos) const;
        Bigint distanceSquared;
        void renderMesh();

        BigVec3 localSize;

        const static Bigint near;
        const static Bigint far;
        static bool disableBrightness;
    };

    class RenderObject2d : public Drawable
    {
    public:
        glm::vec2 position = glm::vec2(0.0f);
        float rotation = 0.0f;
        glm::vec2 scale = glm::vec2(1.0f);

        RenderObject2d(Rendering::Shader *shader, Rendering::Image *im);

        void Update(const float& deltaTime) override {}

        ~RenderObject2d();

        Rendering::Mesh *mesh2d;

    protected:
        void Draw();
    };

    class TextRenderObject : public RenderObject2d {
    public:
        std::string message;
        glm::vec4 font_color;
        TextRenderObject(Rendering::Shader *shader, Rendering::Font *font, const std::string &text, const glm::vec4 &color) : RenderObject2d(shader, Rendering::defaultImageAPI->makeNewImage(font->renderText(text, color))), message(text), font_color(color) {}
        void changeText(Rendering::Font *font, const std::string &text, const glm::vec4 &color) {
            mesh2d->images[0] = Rendering::defaultImageAPI->makeNewImage(font->renderText(text, color));
        }
    };

    static const std::vector<float> cubeVertices  = {
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
    
    static const std::vector<unsigned int> cubeIndices = {
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

    static const std::vector<float> vertices2d = {
        //  x,     y,    u,   v
        -0.5f, -0.5f, 0.0f, 1.0f, // bottom-left
        0.5f, -0.5f, 1.0f, 1.0f,  // bottom-right
        0.5f, 0.5f, 1.0f, 0.0f,   // top-right
        -0.5f, 0.5f, 0.0f, 0.0f   // top-left
    };

    static const std::vector<unsigned int> indices2d = {
        0, 1, 2,
        2, 3, 0};
}