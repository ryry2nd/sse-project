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
        void disableDepthTest();
        void enableDepthTest();
        void enableBackfaceCull();
        void disableBackfaceCull();
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
        GlImage(GLuint id, glm::vec2 size);
        GlImage(SDL_Surface *surface);
        // it unmakes the image
        ~GlImage();
        // it gets the id
        GLuint getID() const;
        GLuint getSID() const;

        void clearTransparent();

    private:
        static void init();
        void setupObject(SDL_Surface *surface);
        GLuint textureID = 0;
        GLuint samplerID = 0;
        static bool hasInit;
    };

    class GlFrameBuffer : public Rendering::FrameBuffer {
    public:
        GlFrameBuffer(glm::vec2 size, uint32_t settings);
        ~GlFrameBuffer();

        void setSize(glm::vec2 size);

        Rendering::Image* getColorImage();
        Rendering::Image* getDepthImage();
        Rendering::Image* getStencilImage();       


        GLuint getID();

    private:
        GLuint id = 0;

        GlImage* colorImage = nullptr;
        GlImage* depthImage = nullptr;
        GlImage* stencilImage = nullptr;
    };

    class GlShader : public Rendering::Shader
    {
    public:
        GlShader(std::string path);
        ~GlShader();

        GLuint getID();

        const std::unordered_map<size_t, GLuint>& getUBOMap() const
        {
            return uboMap;
        }
        const std::unordered_map<size_t, GLuint>& getSSBOMap() const
        {
            return ssboMap;
        }
        const std::unordered_map<size_t, GLuint>& getImageMap() const
        {
            return imageMap;
        }

    protected:
        GLuint id;
        std::unordered_map<size_t, GLuint> uboMap;
        std::unordered_map<size_t, GLuint> ssboMap;
        std::unordered_map<size_t, GLuint> imageMap;
        int compileShaders(std::string path, std::string &vertPath, std::string &fragPath);
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