#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <unordered_map>

#include "../HelperFunctions.hpp"

class HelperFunctionsOpenGl : public HelperFunctions
{
public:
    HelperFunctionsOpenGl(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa = 0, bool fullscreen = false, int vsync = 0, bool hideMouse = true);
    void clearBackground();
    void swapBuffer();
    ~HelperFunctionsOpenGl();

private:
    SDL_GLContext glContext;
};

class ImageOpenGl : public Image
{
public:
    // it makes the image
    ImageOpenGl() {}
    ImageOpenGl(const std::string &filePath);
    ImageOpenGl(SDL_Surface *surface);
    // it unmakes the image
    ~ImageOpenGl();
    // it gets the id
    GLuint getID() const;

    Image *makeNewImage(const std::string &filePath) const;

private:
    void setupObject(SDL_Surface *surface);
    GLuint textureID = 0;
};

class ShaderOpenGl : public Shader
{
public:
    ShaderOpenGl() {}
    ShaderOpenGl(const char *vertexPath, const char *fragmentPath);

    void createUniform(const std::string &location, const UniformTypes &type);
    void includeShader();
    void setUniform(const std::string &location, const float &x);
    void setUniform(const std::string &location, const glm::vec3 &x);
    void setUniform(const std::string &location, const int &x);
    void setUniform(const std::string &location, const glm::mat4 &x);
    void setUniform(const std::string &location, const Image *x);
    void setUniform(const std::string &location, const bool &x);

    Shader *makeNewShader(const char *vertexPath, const char *fragmentPath) const;

    // deletes the thing
    ~ShaderOpenGl();

protected:
    GLuint id;
};

class ComputeShaderOpenGl : public ShaderOpenGl
{
public:
    ComputeShaderOpenGl(const char *computePath);
    ~ComputeShaderOpenGl();
    void setupSsbo(ulong size);
    void dispatch(uint xGroups, uint yGroups, uint zGroups);
    std::vector<char> joinShaderThread(ulong size);

private:
    GLuint ssbo;
};

class OpenGlMesh : public Mesh
{
public:
    OpenGlMesh() {}
    OpenGlMesh(const std::vector<float> &vertices, const std::vector<unsigned int> &indices, const std::vector<short> &vertLogic, const MeshTypes &meshType = MeshTypes::Triangles);
    ~OpenGlMesh();
    // updates the vertices (you dont need to run this unless you changed the vertices)
    void updateVerts(const std::vector<float> &vertices, const std::vector<unsigned int> &indices, const std::vector<short> &vertLogic, const MeshTypes &meshType = MeshTypes::Triangles);
    void Draw();
    Mesh *makeNewMesh(const std::vector<float> &vertices, const std::vector<unsigned int> &indices, const std::vector<short> &vertLogic, const MeshTypes &meshType = MeshTypes::Triangles) const;

    Mesh *makeCopy() const;

    glm::vec3 meshSize;

private:
    glm::vec3 calculateSizes();
    GLuint VAO, VBO, EBO;
    GLenum glMeshType;
    GLsizei size;
};