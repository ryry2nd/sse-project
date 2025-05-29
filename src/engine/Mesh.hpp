#pragma once

#include <vector>
#include <string>
#include "HelperFunctions.hpp"

enum class MeshTypes
{
    Points,
    Lines,
    Triangles
};

class Mesh
{
public:
    virtual ~Mesh() = default;

    virtual void setupObject(const std::vector<float> &verts, const std::vector<short> &vertLogic, MeshTypes meshType = MeshTypes::Triangles) = 0;
    virtual void updateVerts(const std::vector<float> &verts) = 0;
    virtual void includeShader(Shader *shader) = 0;
    virtual void includeTexture(Image *image) = 0;
    virtual void includeFloat(const std::string &location, const float f) = 0;
    virtual void finalizeShaders(const std::vector<float> &vertices) = 0;
    virtual void includeMat4(const std::string &name, const glm::mat4 &mat) = 0;
    virtual void includeTripleFloat(const std::string &location, const float f1, const float f2, const float f3) = 0;
    virtual void includeInt(const std::string &location, const int i) = 0;
    virtual void includeBool(const std::string &location, const bool b) = 0;
    virtual Mesh *makeNewMesh() = 0;

protected:
    Shader *shader = nullptr;
};