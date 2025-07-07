#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Rendering
{
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

    static glm::vec2 getRes()
    {
        SDL_DisplayMode displayMode;
        if (SDL_GetDesktopDisplayMode(0, &displayMode) == 0)
        {
            return glm::vec2(displayMode.w, displayMode.h);
        }
        else
        {
            throw std::runtime_error("get res failed");
        }
    }

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

    protected:
        SDL_Window *window;

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
        virtual void createUniform(const std::string &location, const UniformTypes &type) = 0;
        virtual void includeShader() = 0;
        virtual void setUniform(const std::string &location, const float &x) = 0;
        virtual void setUniform(const std::string &location, const glm::vec3 &x) = 0;
        virtual void setUniform(const std::string &location, const int &x) = 0;
        virtual void setUniform(const std::string &location, const glm::mat4 &x) = 0;
        virtual void setUniform(const std::string &location, const Image *x) = 0;
        virtual void setUniform(const std::string &location, const bool &x) = 0;
        virtual Shader *makeNewShader(const char *vertexPath, const char *fragmentPath) const = 0;

        virtual void enableCulling() = 0;
        virtual void disableCulling() = 0;
    };

    class Mesh
    {
    public:
        virtual ~Mesh() = default;

        virtual void updateVerts(const std::vector<float> &vertices, const std::vector<unsigned int> &indices, const std::vector<short> &vertLogic, const MeshTypes &meshType = MeshTypes::Triangles) = 0;
        virtual void Draw() = 0;
        virtual Mesh *makeNewMesh(const std::vector<float> &vertices, const std::vector<unsigned int> &indices, const std::vector<short> &vertLogic, const MeshTypes &meshType = MeshTypes::Triangles) const = 0;

        virtual Mesh *makeCopy() const = 0;

        glm::vec3 posOffset = glm::vec3(0.0f);
        glm::vec3 sizeOffset = glm::vec3(1.0f);
        glm::vec3 rotOffset = glm::vec3(0.0f);

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
        SDL_Surface *renderText(const std::string &message, SDL_Color color);

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