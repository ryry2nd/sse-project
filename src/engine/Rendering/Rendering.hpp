#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

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
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
        Window(Window&&) noexcept = default;
        Window& operator=(Window&&) noexcept = default;

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


    class Buff {
    public:
        enum class Type {
            Uniform,
            Storage,
            PixelPack,
            PixelUnpack,
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

        std::size_t getSize();
        Type getType();
        Frequency getFrequency();
        
        virtual void write(const std::size_t offset, const std::size_t size, const void* data) = 0;
        virtual void read(const std::size_t offset, const std::size_t size, void* data) = 0;

    protected:
        static bool canRead(Type t);
        static bool canWrite(Type t);
        Type buffType;
        Frequency freq;
        std::size_t allocSize;
    };

    class Image
    {
    public:
        Image& operator=(const Image&) = delete;
        Image& operator=(Image&&) noexcept = default;
        
        virtual ~Image() = default;
        static SDL_Surface *loadFile(const std::string &filePath);
        glm::vec2 getSizes();
    protected:
        glm::vec2 imageSizes;
    };

    class Shader
    {
    public:
        Shader& operator=(const Shader&) = delete;
        Shader& operator=(Shader&&) noexcept = default;


        virtual ~Shader() = default;
    };

    struct Material {
        Shader *shader;
        std::unordered_map<std::string, Image*> images;
        std::unordered_map<std::string, Buff*> buffs;
    };

    struct DrawParams {
        std::unordered_map<std::string, Buff*> buffers;
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

        Mesh& operator=(const Mesh&) = delete;
        Mesh& operator=(Mesh&&) noexcept = default;


        MeshTypes meshType;
        bool backFaceCulling = true;
    };

    namespace CreationFunctions {
        void initAPI(const std::string &apiName);

        // ToDo important. make it so draw is not pulled through the dll every draw call. that might introduce performance issues
        void draw(Material *mat, Mesh *mesh = nullptr, DrawParams *params = nullptr);
        std::unique_ptr<Shader> createShader(const char* vertex, const char* fragment);
        std::unique_ptr<Mesh> createMesh(const float *vertices, const size_t vert_size, const unsigned int *indices, const size_t ind_size, const short *vertLogic, const size_t vert_logic_size, Rendering::Mesh::MeshTypes meshType = Rendering::Mesh::MeshTypes::Triangles);
        std::unique_ptr<Image> createImage(const std::string &filePath);
        std::unique_ptr<Image> createImage(SDL_Surface *surface);
        std::unique_ptr<Window> createWindow(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa = 0, bool fullscreen = false, int vsync = 0, bool hideMouse = true);
        std::unique_ptr<Buff> createBuff(Buff::Type type, Buff::Frequency freq, std::size_t size, const void* data = nullptr);
    }

    //-------- To be deleted --------//
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
    //-------- To be deleted --------//
}