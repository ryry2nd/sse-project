#include "HelperFunctionsOpengl.hpp"
#include <glad/gl.h>

using namespace OpenGl;
using namespace Rendering;

MeshApi::~MeshApi()
{
    delete shader;
    if (VAO != 0)
        glDeleteVertexArrays(1, &VAO);
    if (VBO != 0)
        glDeleteBuffers(1, &VBO);
    if (EBO != 0)
        glDeleteBuffers(1, &EBO);
}

MeshApi::MeshApi(Shader *shady, const std::vector<float> &vertices, const std::vector<unsigned int> &indices, const std::vector<short> &vertLogic, const MeshTypes &meshType) : VAO(0), VBO(0), EBO(0)
{
    shader = shady;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    updateVerts(vertices, indices, vertLogic, meshType);
}

void MeshApi::updateVerts(const std::vector<float> &vertices,
                          const std::vector<unsigned int> &indices,
                          const std::vector<short> &vertLogic,
                          const MeshTypes &meshType)
{
    glBindVertexArray(VAO); // Make sure the VAO is bound

    // Upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Upload index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Set vertex attribute pointers
    int floatsPerVert = 0;
    for (short s : vertLogic)
        floatsPerVert += s;

    size_t numsSoFar = 0;
    for (size_t i = 0; i < vertLogic.size(); i++)
    {
        glVertexAttribPointer(i, vertLogic[i], GL_FLOAT, GL_FALSE, floatsPerVert * sizeof(float), (void *)(numsSoFar * sizeof(float)));
        glEnableVertexAttribArray(i);
        numsSoFar += vertLogic[i];
    }

    this->vertices = vertices;
    this->indices = indices;
    this->vertLogic = vertLogic;
    this->meshType = meshType;

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

    meshSize = calculateSizes();
}

void MeshApi::Draw()
{
    shader->setImages(images);
    glBindVertexArray(VAO); // ngl who knows what this crap means, according to the names it applies and binds stuff
    glDrawElements(glMeshType, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

glm::vec3 MeshApi::calculateSizes()
{
    glm::vec3 minCorner(FLT_MAX);
    glm::vec3 maxCorner(-FLT_MAX);

    int total = 0;
    for (short &i : vertLogic)
        total += i;

    glm::vec3 pos;

    for (size_t i = 0; i < vertices.size(); i += total)
    {
        if (vertLogic[0] == 3)
        {
            pos = glm::vec3(vertices[i], vertices[i + 1], vertices[i + 2]);
        }
        else
        {
            pos = glm::vec3(vertices[i], vertices[i + 1], 0);
        }
        minCorner = glm::min(minCorner, pos);
        maxCorner = glm::max(maxCorner, pos);
    }

    return maxCorner - minCorner;
}

Mesh *MeshApi::makeNewMesh(Shader *shady, const std::vector<float> &vertices, const std::vector<unsigned int> &indices, const std::vector<short> &vertLogic, const MeshTypes &meshType) const
{
    return new MeshApi(shady, vertices, indices, vertLogic, meshType);
}