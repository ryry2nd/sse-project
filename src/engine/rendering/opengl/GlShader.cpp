#include "HelperFunctionsOpengl.hpp"
#include <glad/gl.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <memory>

using namespace OpenGl;
using namespace Rendering;

GlShader::GlShader(const char *vertexPath, const char *fragmentPath)
{
    if (!std::filesystem::exists(vertexPath)) {
        std::cerr << "Shader path: " << vertexPath << " does not exist\n";
        return;
    }
    if (!std::filesystem::exists(fragmentPath)) {
        std::cerr << "Shader path: " << fragmentPath << " does not exist\n";
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
    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);

    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(id, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << '\n';
    }

    // Delete shaders; linked into program now and no longer needed
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

GlShader::~GlShader()
{
    if (UBO != 0)
        glDeleteBuffers(1, &UBO);
    if (id != 0)
        glDeleteProgram(id);
}

void GlShader::setUniform(const std::string &location, const float &x)
{
    glUniform1f(glGetUniformLocation(id, location.c_str()), x);
}

void GlShader::setUniform(const std::string &location, const glm::vec3 &x)
{
    glUniform3f(glGetUniformLocation(id, location.c_str()), x.x, x.y, x.z);
}

void GlShader::setUniform(const std::string &location, const int &x)
{
    glUniform1i(glGetUniformLocation(id, location.c_str()), x);
}

void GlShader::setUniform(const std::string &location, const bool &x)
{
    glUniform1f(glGetUniformLocation(id, location.c_str()), x);
}

void GlShader::setUniform(const std::string &location, const glm::mat4 &x)
{
    glUniformMatrix4fv(glGetUniformLocation(id, location.c_str()), 1, GL_FALSE, glm::value_ptr(x));
}

void GlShader::setImages(std::vector<Image *> &textures) {
    for (int i = 0; i < textures.size(); i++) {
        GLuint id = static_cast<GlImage*>(textures[i])->getID();
        glActiveTexture(GL_TEXTURE0 + i + 1);
        glBindTexture(GL_TEXTURE_2D, id);

        char uniformName[16];
        snprintf(uniformName, sizeof(uniformName), "texture%d", (i + 1) % 1000);

        GLint loc = glGetUniformLocation(id, uniformName);
        if (loc != -1) {
            glUniform1i(loc, i + 1);
        }
    }
}

void GlShader::SetShader() {
    glUseProgram(id);

    if (cullingEnabled) {
        glEnable(GL_CULL_FACE);
    }
    else {
        glDisable(GL_CULL_FACE);
    }

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);
}