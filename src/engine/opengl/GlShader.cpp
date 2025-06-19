#include "HelperFunctionsOpengl.hpp"

ShaderOpenGl::ShaderOpenGl(const char *vertexPath, const char *fragmentPath)
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

ShaderOpenGl::~ShaderOpenGl()
{
    glDeleteProgram(id);
}

void ShaderOpenGl::createUniform(const std::string &location, const UniformTypes &type)
{
}

void ShaderOpenGl::setUniform(const std::string &location, const float &x)
{
    glUniform1f(glGetUniformLocation(id, location.c_str()), x);
}

void ShaderOpenGl::setUniform(const std::string &location, const glm::vec3 &x)
{
    glUniform3f(glGetUniformLocation(id, location.c_str()), x.x, x.y, x.z);
}

void ShaderOpenGl::setUniform(const std::string &location, const int &x)
{
    glUniform1f(glGetUniformLocation(id, location.c_str()), x);
}

void ShaderOpenGl::setUniform(const std::string &location, const bool &x)
{
    glUniform1f(glGetUniformLocation(id, location.c_str()), x);
}

void ShaderOpenGl::setUniform(const std::string &location, const glm::mat4 &x)
{
    glUniformMatrix4fv(glGetUniformLocation(id, location.c_str()), 1, GL_FALSE, glm::value_ptr(x));
}

void ShaderOpenGl::setUniform(const std::string &location, const Image *image)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, static_cast<const ImageOpenGl *>(image)->getID());
    glUniform1i(glGetUniformLocation(id, location.c_str()), 0);
}

void ShaderOpenGl::includeShader()
{
    glUseProgram(id);
}