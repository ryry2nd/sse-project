#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

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

class HelperFunctions
{
public:
    HelperFunctions(glm::vec2 res, const char *name, Uint32 flags, Uint32 aa = 0, bool fullscreen = false, bool hideMouse = false)
    {
        this->res = res;
        // it initialises sdl
        if (SDL_Init(SDL_INIT_VIDEO) != 0)
        {
            std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
            throw std::runtime_error("Sdl cant initialise");
        }
        if (TTF_Init() == -1)
        {
            SDL_Log("TTF_Init error: %s", TTF_GetError());
        }

        if (aa >= 0)
        {
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, aa);
        }

        window = SDL_CreateWindow(name, 100, 100, res.x, res.y, flags);
        if (!window)
        {
            std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << "\n";
            SDL_Quit();
            throw std::runtime_error("cant create sdl window");
        }

        if (hideMouse)
            SDL_SetRelativeMouseMode(SDL_TRUE); // hides the mouse
        if (fullscreen)
            SDL_MaximizeWindow(window);
    }
    virtual void clearBackground() = 0;

    virtual void swapBuffer() = 0;

    ~HelperFunctions()
    {
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

protected:
    SDL_Window *window;
    glm::vec2 res;
};

class Image
{
public:
    virtual ~Image() = default;
    virtual Image *makeNewImage(const std::string &filePath) const = 0;
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

    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    std::vector<short> vertLogic;
    MeshTypes meshType;
};

class Font
{
public:
    Font(const std::string &fontPath, int size)
    {
        font = TTF_OpenFont(fontPath.c_str(), size);
        if (!font)
            throw std::runtime_error("Failed to load font \"" + fontPath + "\": " + TTF_GetError());
    }

    ~Font()
    {
        TTF_CloseFont(font);
    }

    SDL_Surface *renderText(const std::string &message, SDL_Color color)
    {
        if (!font)
            throw std::runtime_error("no font defined");

        SDL_Surface *surf = TTF_RenderUTF8_Blended(font, message.c_str(), color);
        if (!surf)
            throw std::runtime_error("surface failed to initialise");

        SDL_Surface *formattedSurf = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_ABGR8888, 0);
        SDL_FreeSurface(surf);
        return formattedSurf;
    }

private:
    TTF_Font *font;
};