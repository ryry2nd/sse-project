#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Rendering.hpp"

typedef struct SDL_GLContextState *SDL_GLContext;

namespace OpenGl
{
    using GLuint = unsigned int;
    using GLenum = unsigned int;
    using GLsizei = int;

    class GlWindow : public Rendering::Window
    {
    public:
        GlWindow(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa = 0, bool fullscreen = false, int vsync = 0, bool hideMouse = true);
        void clearBackground();
        void swapBuffer();
        void updateScreenRes();
        ~GlWindow();

    private:
        SDL_GLContext glContext;
    };

    class GlImage : public Rendering::Image
    {
    public:
        // it makes the image
        GlImage(const std::string &filePath);
        GlImage(SDL_Surface *surface);
        // it unmakes the image
        ~GlImage();
        // it gets the id
        GLuint getID() const;

    private:
        void setupObject(SDL_Surface *surface);
        GLuint textureID = 0;
    };

    class GlShader : public Rendering::Shader
    {
    public:
        GlShader(const char *vertexPath, const char *fragmentPath);
        ~GlShader();

        void setUniform(const std::string &location, const float &x);
        void setUniform(const std::string &location, const glm::vec3 &x);
        void setUniform(const std::string &location, const int &x);
        void setUniform(const std::string &location, const glm::mat4 &x);
        void setUniform(const std::string &location, const bool &x);

        void setImages(std::vector<Rendering::Image*> &textures);
        void SetShader();  

    protected:
        GLuint id;
        GLuint UBO;
    };

    class GlMesh : public Rendering::Mesh
    {
    public:
        GlMesh(Rendering::Shader *shady, const float *vertices, const size_t vert_size, const unsigned int *indices, const size_t ind_size, const short *vertLogic, const size_t vert_logic_size, const Rendering::MeshTypes &meshType = Rendering::MeshTypes::Triangles);
        ~GlMesh();
        // updates the vertices (you dont need to run this unless you changed the vertices)
        void updateVerts(const float *vertices, const size_t vert_size, const unsigned int *indices, const size_t ind_size, const short *vertLogic, const size_t vert_logic_size, const Rendering::MeshTypes &meshType = Rendering::MeshTypes::Triangles);
        void Draw();
        glm::vec3 meshSize;

    private:
        size_t ind_size;
        GLuint VAO, VBO, EBO;
        GLenum glMeshType;
        GLsizei size;
    };
}