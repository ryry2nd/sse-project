#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

#define CAMERA_PREALLOC 5
#define OBJECT_PREALLOC 100

struct TTF_Font;
struct SDL_Surface;
struct SDL_Window;
struct SDL_Color;

typedef uint32_t Uint32;
typedef uint64_t Uint64;
typedef uint8_t Uint8;

namespace Rendering
{
    SDL_Color Vec4ToSDLColor(const glm::vec4& color);

    class Window
    {
    public:
        Window(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa = 0, bool fullscreen = false, bool hideMouse = false);
        virtual ~Window() = 0;


        virtual void clearBackground() = 0;
        virtual void swapBuffer() = 0;
        virtual void updateScreenRes() = 0;
        static void Update();
        static const bool *getKeystates(int &numKeys);

        static float deltaTime;
        static double fps;
        static Uint64 now;
        static glm::vec2 res;

        static void init();
        static void shutdown();

    protected:
        static SDL_Window *window;

    private:
        static Uint64 lastCounter;
    };

    inline std::vector<std::unique_ptr<Window>> sdlWindows;

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
        enum class MeshTypes
        {
            Points,
            Lines,
            Triangles
        };
        virtual ~Mesh() = default;

        virtual void updateVerts(const float *vertices, const size_t vert_size, const unsigned int *indices, const size_t ind_size, const short *vertLogic, const size_t vert_logic_size, const MeshTypes &meshType = MeshTypes::Triangles) = 0;
        virtual void Draw() = 0;

        glm::vec3 posOffset = glm::vec3(0.0f);
        glm::vec3 sizeOffset = glm::vec3(1.0f);
        glm::vec3 rotOffset = glm::vec3(0.0f);

        Shader *shader;

        std::vector<Image *> images;

    protected:
        MeshTypes meshType;
    };

    class Font
    {
    public:
        static void init();
        static void deinit();
        Font(const std::string &fontPath, int size);
        ~Font();
        SDL_Surface *renderText(const std::string &message, const glm::vec4 &color);

    private:
        TTF_Font *font;
    };
    
    class Buff {
    public:
        enum class Type {
            Array,
            Element,
            Uniform,
            Storage,
            PixelPack,
            PixelUnpack
        };
        enum class Frequency {
            Static,
            Dynamic,
            Stream
        };

        Buff(Type type, Frequency freq, std::size_t size);
        virtual ~Buff() = default;
        Buff(const Buff&) = delete;
        Buff& operator=(const Buff&) = delete;
        Buff(Buff&&) noexcept = default;
        Buff& operator=(Buff&&) noexcept = default;
        
        virtual void write(const std::size_t offset, const std::size_t size, const void* data) = 0;
        virtual void read(const std::size_t offset, const std::size_t size, void* outData) = 0;

    private:
        Type buffType;
        Frequency freq;
        std::size_t allocSize;
    };

    namespace CreationFunctions {
        void initAPI(const std::string &apiName);
        std::unique_ptr<Shader> createShader(const char* vertex, const char* fragment);
        std::unique_ptr<Mesh> createMesh(Rendering::Shader *shady, const float *vertices, const size_t vert_size, const unsigned int *indices, const size_t ind_size, const short *vertLogic, const size_t vert_logic_size, const Rendering::Mesh::MeshTypes &meshType = Rendering::Mesh::MeshTypes::Triangles);
        std::unique_ptr<Image> createImage(const std::string &filePath);
        std::unique_ptr<Image> createImage(SDL_Surface *surface);
        std::unique_ptr<Window> createWindow(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa = 0, bool fullscreen = false, int vsync = 0, bool hideMouse = true);
        std::unique_ptr<Buff> createBuff(Buff::Type type, Buff::Frequency freq, std::size_t size);
    }
}