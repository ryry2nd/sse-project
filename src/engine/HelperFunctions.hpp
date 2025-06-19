#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

enum class MeshTypes
{
    Points,
    Lines,
    Triangles
};

enum class UniformTypes
{
    Float,
    Mat4,
    Vec3,
    Int,
    Bool
};

class HelperFunctions
{
public:
    virtual void clearBackground() = 0;

    virtual void swapBuffer() = 0;

    virtual ~HelperFunctions() = default;

protected:
    SDL_Window *window;
};

class Image
{
public:
    virtual ~Image() = default;
};

class Shader
{
public:
    // deletes the thing

    virtual ~Shader() = default;
    virtual void createUniform(const std::string &location, const UniformTypes &type) = 0;
    virtual void includeShader() = 0;
    virtual void setUniform(const std::string &location, const float &x) = 0;
    virtual void setUniform(const std::string &location, const glm::vec3 &x) = 0;
    virtual void setUniform(const std::string &location, const int &x) = 0;
    virtual void setUniform(const std::string &location, const glm::mat4 &x) = 0;
    virtual void setUniform(const std::string &location, const Image *x) = 0;
    virtual void setUniform(const std::string &location, const bool &x) = 0;
};

class Mesh
{
public:
    virtual ~Mesh() = default;

    virtual void setupObject(const std::vector<float> &vertices, const std::vector<short> &vertLogic, const MeshTypes &meshType = MeshTypes::Triangles) = 0;
    virtual void updateVerts(const std::vector<float> &vertices, const std::vector<short> &vertLogic, const MeshTypes &meshType = MeshTypes::Triangles) = 0;
    virtual void finalizeShaders() = 0;
    virtual Mesh *makeNewMesh() = 0;
};