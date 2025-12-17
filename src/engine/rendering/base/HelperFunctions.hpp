#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

struct TTF_Font;
struct SDL_Surface;
struct SDL_Window;

typedef uint32_t Uint32;
typedef uint64_t Uint64;

namespace Rendering
{
    enum class MeshTypes
    {
        Points,
        Lines,
        Triangles
    };

    class HelperFunctions
    {
    public:
        HelperFunctions(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa = 0, bool fullscreen = false, bool hideMouse = false);
        virtual void clearBackground() = 0;
        virtual void swapBuffer() = 0;
        virtual void updateScreenRes() = 0;
        static void Update();
        
        ~HelperFunctions();

        static float deltaTime;
        static double fps;
        static Uint64 now;
        static glm::vec2 res;

    protected:
        static SDL_Window *window;

    private:
        static Uint64 lastCounter;
    };

    class Image
    {
    public:
        virtual ~Image() = default;
        virtual Image *makeNewImage(const std::string &filePath) const = 0;
        virtual Image *makeNewImage(SDL_Surface *surface) const = 0;
        static SDL_Surface *loadFile(const std::string &filePath);
        glm::vec2 imageSizes;
    };

    class Shader
    {
    public:
        // deletes the thing
        virtual ~Shader() = default;

        virtual void setUniform(const std::string &location, const float &x) = 0;
        virtual void setUniform(const std::string &location, const glm::vec3 &x) = 0;
        virtual void setUniform(const std::string &location, const int &x) = 0;
        virtual void setUniform(const std::string &location, const glm::mat4 &x) = 0;
        virtual void setUniform(const std::string &location, const bool &x) = 0;
        
        virtual void setImages(std::vector<Image *> &textures) = 0;
        virtual void SetShader() = 0;

        virtual Shader *makeNewShader(const char *vertexPath, const char *fragmentPath) const = 0;

        // enables or disables culling of back face, disable if object is 2d
        bool cullingEnabled = true;
    };

    class Mesh
    {
    public:
        virtual ~Mesh() = default;

        virtual void updateVerts(const std::vector<float> &vertices, const std::vector<unsigned int> &indices, const std::vector<short> &vertLogic, const MeshTypes &meshType = MeshTypes::Triangles) = 0;
        virtual void Draw() = 0;
        
        virtual Mesh *makeNewMesh(Shader *shader, const std::vector<float> &vertices, const std::vector<unsigned int> &indices, const std::vector<short> &vertLogic, const MeshTypes &meshType = MeshTypes::Triangles) const = 0;

        glm::vec3 posOffset = glm::vec3(0.0f);
        glm::vec3 sizeOffset = glm::vec3(1.0f);
        glm::vec3 rotOffset = glm::vec3(0.0f);

        Shader *shader;

        std::vector<Image *> images;

    protected:
        std::vector<float> vertices;
        std::vector<unsigned int> indices;
        std::vector<short> vertLogic;
        MeshTypes meshType;
    };

    class Font
    {
    public:
        Font(const std::string &fontPath, int size);
        ~Font();
        SDL_Surface *renderText(const std::string &message, const glm::vec4 &color);

    private:
        TTF_Font *font;
    };

    inline Mesh *defaultMeshAPI;
    inline Shader *defaultShaderAPI;
    inline Image *defaultImageAPI;

    static void init(Mesh *meshApi, Shader *shaderApi, Image *imageApi)
    {
        defaultMeshAPI = meshApi;
        defaultShaderAPI = shaderApi;
        defaultImageAPI = imageApi;
    }
}