#include "GlRendering.hpp"

#include <spdlog/spdlog.h>
#include <glad/gl.h>
#include <fstream>
#include <sstream>
#include <filesystem>

using namespace OpenGl;
using namespace Rendering;

GlShader::GlShader(const char *vertexPath, const char *fragmentPath)
{
    if (!std::filesystem::exists(vertexPath)) {
        spdlog::error("Shader path: {} does not exist", vertexPath);
        return;
    }
    if (!std::filesystem::exists(fragmentPath)) {
        spdlog::error("Shader path: {} does not exist", fragmentPath);
        return;
    }

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
        spdlog::error("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ");
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
        spdlog::error("ERROR::SHADER::VERTEX::COMPILATION_FAILED: {}", infoLog);
    }

    // Fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
        spdlog::error("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED: {}", infoLog);
    }

    // Shader Program
    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);

    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(id, 512, nullptr, infoLog);
        spdlog::error("ERROR::SHADER::PROGRAM::LINKING_FAILED: {}", infoLog);
    }

    // Delete shaders; linked into program now and no longer needed
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    
    // GLint blockCount = 0;
    // glGetProgramiv(id, GL_ACTIVE_UNIFORM_BLOCKS, &blockCount);

    // for (GLuint i = 0; i < (GLuint)blockCount; i++)
    // {
    //     char name[128];
    //     glGetActiveUniformBlockName(id, i, sizeof(name), nullptr, name);
    //     glUniformBlockBinding(id, i, i);
    //     bindingMap[name] = i;
    // }

    GLuint nextBinding = 0;

    // ====================================================
    // UBO REFLECTION
    // ====================================================

    GLint uniformBlockCount = 0;
    glGetProgramInterfaceiv(id, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &uniformBlockCount);

    for (GLuint i = 0; i < (GLuint)uniformBlockCount; i++)
    {
        char name[256];
        GLsizei length = 0;

        glGetProgramResourceName(id,
                                 GL_UNIFORM_BLOCK,
                                 i,
                                 sizeof(name),
                                 &length,
                                 name);

        glUniformBlockBinding(id, i, nextBinding);
        bindingMap[name] = nextBinding;

        nextBinding++;
    }

    // ====================================================
    // SSBO REFLECTION
    // ====================================================
    GLint storageBlockCount = 0;
    glGetProgramInterfaceiv(id, GL_SHADER_STORAGE_BLOCK, GL_ACTIVE_RESOURCES, &storageBlockCount);

    for (GLuint i = 0; i < (GLuint)storageBlockCount; i++)
    {
        char name[256];
        GLsizei length = 0;

        glGetProgramResourceName(id,
                                 GL_SHADER_STORAGE_BLOCK,
                                 i,
                                 sizeof(name),
                                 &length,
                                 name);

        glShaderStorageBlockBinding(id, i, nextBinding);
        bindingMap[name] = nextBinding;

        nextBinding++;
    }
}

GlShader::~GlShader()
{
    if (id != 0)
        glDeleteProgram(id);
}

GLuint GlShader::getID() {
    return id;
}