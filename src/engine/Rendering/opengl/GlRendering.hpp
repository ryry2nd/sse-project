#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <engine/Rendering/Rendering.hpp>

typedef struct SDL_GLContextState *SDL_GLContext;

namespace OpenGl
{
    using GLuint = unsigned int;
    using GLenum = unsigned int;
    using GLsizei = int;

    class GlBuff : public Rendering::Buff {
    public:
        GlBuff(Type type, Frequency freq, std::size_t size, const void* data = nullptr);
        ~GlBuff();
        void write(const std::size_t offset, const std::size_t size, const void* data);
        void read(const std::size_t offset, const std::size_t size, void* data);

        GLuint getID();
        GLenum getTarget();
        GLenum getUsage();
    private:
        GLuint id;
        GLenum target;
        GLenum usage;

        GLenum toGLTarget(Type type);
        GLenum toGLUsage(Frequency freq);
    };

    class GlWindow : public Rendering::Window
    {
    public:
        GlWindow(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa = 0, bool fullscreen = false, int vsync = 0, bool hideMouse = true);
        void clearBackground();
        void swapBuffer();
        void updateScreenRes();
        void setBackgroundColor(glm::vec4 color);
        ~GlWindow();

        SDL_GLContext getContext();
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

        GLuint getID();

        const std::unordered_map<std::string, GLuint>& getBindingMap() const
        {
            return bindingMap;
        }

    protected:
        GLuint id;
        std::unordered_map<std::string, GLuint> bindingMap;
    };

    class GlMesh : public Rendering::Mesh
    {
    public:
        GlMesh(const float *vertices, const size_t vert_size, const unsigned int *indices, const size_t ind_size, const short *vertLogic, const size_t vert_logic_size, Rendering::Mesh::MeshTypes meshType = Rendering::Mesh::MeshTypes::Triangles);
        ~GlMesh();
        
        void updateVerts(const float *vertices, const size_t vert_size, const unsigned int *indices, const size_t ind_size, const short *vertLogic, const size_t vert_logic_size, Rendering::Mesh::MeshTypes meshType = Rendering::Mesh::MeshTypes::Triangles);

        glm::vec3 getMeshSize();
        GLuint getVAO();
        GLuint getVBO();
        GLuint getEBO();
        GLenum getMeshType();
        GLsizei getSize();

        size_t getInd();
        size_t getVert();
    private:
        glm::vec3 meshSize;
        size_t ind_size;
        size_t vert_size;
        GLuint VAO, VBO, EBO;
        GLenum glMeshType;
        GLsizei size;
    };

    void draw(Rendering::Material *mat, Rendering::Mesh *mesh, Rendering::DrawParams *params);
}