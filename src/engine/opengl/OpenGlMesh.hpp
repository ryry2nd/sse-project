#pragma once

#include <vector>
#include "../Mesh.hpp"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class OpenGlMesh : public Mesh
{
public:
    OpenGlMesh() : VAO(0), VBO(0) {}

    int floatsPerVert;

    ~OpenGlMesh()
    {
        if (VAO != 0)
            glDeleteVertexArrays(1, &VAO);
        if (VBO != 0)
            glDeleteBuffers(1, &VBO);
    }

    void setupObject(const std::vector<float> &vertices, const std::vector<short> &vertLogic, MeshTypes meshType = MeshTypes::Triangles)
    {
        if (VAO != 0)
            glDeleteVertexArrays(1, &VAO);
        if (VBO != 0)
            glDeleteBuffers(1, &VBO);

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        floatsPerVert = 0;

        for (short s : vertLogic)
        {
            floatsPerVert += s;
        }

        size_t numsSoFar = 0;

        for (size_t i = 0; i < vertLogic.size(); i++)
        {
            glVertexAttribPointer(i, vertLogic[i], GL_FLOAT, GL_FALSE, floatsPerVert * sizeof(float), (void *)(numsSoFar * sizeof(float)));
            glEnableVertexAttribArray(i);
            numsSoFar += vertLogic[i];
        }

        switch (meshType)
        {
        case MeshTypes::Points:
            glMeshType = GL_POINTS;
            break;
        case MeshTypes::Lines:
            glMeshType = GL_LINES;
            break;
        case MeshTypes::Triangles:
            glMeshType = GL_TRIANGLES;
            break;
        }
    }

    // updates the vertices (you dont need to run this unless you changed the vertices)
    void updateVerts(const std::vector<float> &vertices)
    {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
    }

    void includeShader(Shader *shader)
    {
        this->shader = shader;
        glUseProgram(shader->getShader());
    }

    void includeMat4(const std::string &name, const glm::mat4 &mat)
    {
        glUniformMatrix4fv(glGetUniformLocation(shader->getShader(), name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }

    void includeTexture(Image *image)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, image->getID());

        GLuint texLoc = glGetUniformLocation(shader->getShader(), "texture1");
        if (texLoc != -1)
            glUniform1i(texLoc, 0);
    }

    void includeFloat(const std::string &location, const float f)
    {
        glUniform1f(glGetUniformLocation(shader->getShader(), location.c_str()), f);
    }

    void includeTripleFloat(const std::string &location, const float f1, const float f2, const float f3)
    {
        glUniform3f(glGetUniformLocation(shader->getShader(), location.c_str()), f1, f2, f3);
    }

    void includeInt(const std::string &location, const int i)
    {
        glUniform1i(glGetUniformLocation(shader->getShader(), location.c_str()), i);
    }

    void includeBool(const std::string &location, const bool b)
    {
        glUniform1i(glGetUniformLocation(shader->getShader(), location.c_str()), b);
    }

    void finalizeShaders(const std::vector<float> &vertices)
    {
        glBindVertexArray(VAO); // ngl who knows what this crap means, according to the names it applies and binds stuff
        glDrawArrays(glMeshType, 0, vertices.size() / floatsPerVert);
        glBindVertexArray(0);
    }

    Mesh *makeNewMesh()
    {
        return new OpenGlMesh();
    }

private:
    GLuint VAO, VBO;
    GLenum glMeshType;
};