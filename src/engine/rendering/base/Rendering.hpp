#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

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

    class Window
    {
    public:
        Window(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa = 0, bool fullscreen = false, bool hideMouse = false);
        virtual void clearBackground() = 0;
        virtual void swapBuffer() = 0;
        virtual void updateScreenRes() = 0;
        static void Update();
        static const bool *getKeystates(int &numKeys);
        
        ~Window();

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

        // enables or disables culling of back face, disable if object is 2d
        bool cullingEnabled = true;
    };

    class Mesh
    {
    public:
        virtual ~Mesh() = default;

        virtual void updateVerts(const std::vector<float> &vertices, const std::vector<unsigned int> &indices, const std::vector<short> &vertLogic, const MeshTypes &meshType = MeshTypes::Triangles) = 0;
        virtual void Draw() = 0;

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

    inline std::vector<std::unique_ptr<Window>> sdlWindows;

    namespace CreationFunctions {
        void initAPI(const std::string &apiName);
        std::unique_ptr<Shader> createShader(const char* vertex, const char* fragment);
        std::unique_ptr<Mesh> createMesh(Rendering::Shader *shady, const std::vector<float> &vertices, const std::vector<unsigned int> &indices, const std::vector<short> &vertLogic, const Rendering::MeshTypes &meshType = Rendering::MeshTypes::Triangles);
        std::unique_ptr<Image> createImage(const std::string &filePath);
        std::unique_ptr<Image> createImage(SDL_Surface *surface);
        std::unique_ptr<Window> createWindow(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa = 0, bool fullscreen = false, int vsync = 0, bool hideMouse = true);
    }
}