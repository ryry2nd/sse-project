#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "../HelperFunctions.hpp"

class HelperFunctionsOpenGl : public HelperFunctions
{
public:
    HelperFunctionsOpenGl(SDL_Window *window)
    {
        this->window = window;
        // this does what it says
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        // this makes the opengl content
        glContext = SDL_GL_CreateContext(window);
        if (!glContext)
        {
            std::cerr << "SDL_GL_CreateContext Error: " << SDL_GetError() << "\n";
            SDL_DestroyWindow(window);
            SDL_Quit();
            throw std::runtime_error("Had issues making the opengl context");
        }

        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK)
        {
            std::cerr << "Failed to initialize GLEW\n";
            SDL_GL_DeleteContext(glContext);
            SDL_DestroyWindow(window);
            SDL_Quit();
            throw std::runtime_error("Failed to initialize GLEW\n");
        }

        // this turns on all the cool opengl settings that probably should be the default
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_STENCIL_TEST);
        glEnable(GL_PROGRAM_POINT_SIZE);
        glDepthFunc(GL_LESS); // or GL_LEQUAL
        glDepthMask(GL_TRUE); // enable writing to depth buffer
    }

    void clearBackground()
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void swapBuffer()
    {
        SDL_GL_SwapWindow(window);
    }

    ~HelperFunctionsOpenGl()
    {
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

private:
    SDL_Window *window;
    SDL_GLContext glContext;
};

class ImageOpenGl : public Image
{
public:
    // it makes the image
    ImageOpenGl(const std::string &filePath)
    {
        SDL_Surface *surface = IMG_Load(filePath.c_str());
        if (!surface)
        {
            std::cerr << "Image load fail: " << IMG_GetError() << "\n";
            return;
        }

        GLenum format = surface->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0,
                     format, GL_UNSIGNED_BYTE, surface->pixels);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        SDL_FreeSurface(surface);
    }

    // it unmakes the image
    ~ImageOpenGl()
    {
        if (textureID)
            glDeleteTextures(1, &textureID);
    }

    // it gets the id
    unsigned int getID() const
    {
        return textureID;
    }

private:
    GLuint textureID = 0;
};

class ShaderOpenGl : public Shader
{
public:
    ShaderOpenGl(const char *vertexPath, const char *fragmentPath)
    {
        std::string vertexCode, fragmentCode;
        std::ifstream vShaderFile, fShaderFile;

        // Ensure ifstream objects can throw exceptions
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try
        {
            // Open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;

            // Read file’s buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();

            // Close files
            vShaderFile.close();
            fShaderFile.close();

            // Convert streams into strings
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure &e)
        {
            std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n";
        }

        const char *vShaderCode = vertexCode.c_str();
        const char *fShaderCode = fragmentCode.c_str();

        // Compile shaders
        GLuint vertex, fragment;
        int success;
        char infoLog[512];

        // Vertex Shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, nullptr);
        glCompileShader(vertex);
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
            std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                      << infoLog << '\n';
        }

        // Fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, nullptr);
        glCompileShader(fragment);
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
            std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                      << infoLog << '\n';
        }

        // Shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);

        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(ID, 512, nullptr, infoLog);
            std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                      << infoLog << '\n';
        }

        // Delete shaders; linked into program now and no longer needed
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    // gets the shader id
    unsigned int getShader() const
    {
        return ID;
    }

    // deletes the thing
    ~ShaderOpenGl()
    {
        glDeleteProgram(ID);
    }

private:
    GLuint ID;
};