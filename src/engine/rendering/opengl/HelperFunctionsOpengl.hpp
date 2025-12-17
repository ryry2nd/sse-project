#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "HelperFunctions.hpp"

typedef struct SDL_GLContextState *SDL_GLContext;

namespace OpenGl
{
    using GLuint = unsigned int;
    using GLenum = unsigned int;
    using GLsizei = int;

    class HelperFunctionsApi : public Rendering::HelperFunctions
    {
    public:
        HelperFunctionsApi(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa = 0, bool fullscreen = false, int vsync = 0, bool hideMouse = true);
        void clearBackground();
        void swapBuffer();
        void updateScreenRes();
        ~HelperFunctionsApi();

    private:
        SDL_GLContext glContext;
    };

    class ImageApi : public Rendering::Image
    {
    public:
        // it makes the image
        ImageApi() {}
        ImageApi(const std::string &filePath);
        ImageApi(SDL_Surface *surface);
        // it unmakes the image
        ~ImageApi();
        // it gets the id
        GLuint getID() const;

        Rendering::Image *makeNewImage(const std::string &filePath) const;
        Rendering::Image *makeNewImage(SDL_Surface *surface) const;

    private:
        void setupObject(SDL_Surface *surface);
        GLuint textureID = 0;
    };

    class ShaderApi : public Rendering::Shader
    {
    public:
        ShaderApi() {}
        ShaderApi(const char *vertexPath, const char *fragmentPath);
        ~ShaderApi();

        void setUniform(const std::string &location, const float &x);
        void setUniform(const std::string &location, const glm::vec3 &x);
        void setUniform(const std::string &location, const int &x);
        void setUniform(const std::string &location, const glm::mat4 &x);
        void setUniform(const std::string &location, const bool &x);

        void setImages(std::vector<Rendering::Image*> &textures);
        void SetShader();

        Rendering::Shader *makeNewShader(const char *vertexPath, const char *fragmentPath) const;        

    protected:
        GLuint id;
        GLuint UBO;
    };

    class MeshApi : public Rendering::Mesh
    {
    public:
        MeshApi() {}
        MeshApi(Rendering::Shader *shady, const std::vector<float> &vertices, const std::vector<unsigned int> &indices, const std::vector<short> &vertLogic, const Rendering::MeshTypes &meshType = Rendering::MeshTypes::Triangles);
        ~MeshApi();
        // updates the vertices (you dont need to run this unless you changed the vertices)
        void updateVerts(const std::vector<float> &vertices, const std::vector<unsigned int> &indices, const std::vector<short> &vertLogic, const Rendering::MeshTypes &meshType = Rendering::MeshTypes::Triangles);
        void Draw();
        Rendering::Mesh *makeNewMesh(Rendering::Shader *shady, const std::vector<float> &vertices, const std::vector<unsigned int> &indices, const std::vector<short> &vertLogic, const Rendering::MeshTypes &meshType = Rendering::MeshTypes::Triangles) const;
        glm::vec3 meshSize;

    private:
        glm::vec3 calculateSizes();
        GLuint VAO, VBO, EBO;
        GLenum glMeshType;
        GLsizei size;
    };
}