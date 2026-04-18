#include "GlRendering.hpp"
#include <glad/gl.h>

using namespace OpenGl;
using namespace Rendering;

glm::vec3 calculateSizes(const float *vertices, const size_t vert_size,
                          const unsigned int *indices, const size_t ind_size,
                          const short *vertLogic, const size_t vert_logic_size)
{
    glm::vec3 minCorner(FLT_MAX);
    glm::vec3 maxCorner(-FLT_MAX);

    int total = 0;
    for (size_t i = 0; i < vert_logic_size; i++)
        total += vertLogic[i];

    glm::vec3 pos;

    for (size_t i = 0; i < vert_size; i += total)
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

GlMesh::~GlMesh()
{
    if (VAO != 0)
        glDeleteVertexArrays(1, &VAO);
    if (VBO != 0)
        glDeleteBuffers(1, &VBO);
    if (EBO != 0)
        glDeleteBuffers(1, &EBO);
}

GlMesh::GlMesh(Shader *shady, const float *vertices, const size_t vert_size,
                const unsigned int *indices, const size_t ind_size,
                const short *vertLogic, const size_t vert_logic_size,
                const MeshTypes &meshType
            ) : VAO(0), VBO(0), EBO(0)
{
    shader = shady;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    updateVerts(vertices, vert_size, indices, ind_size, vertLogic, vert_logic_size, meshType);
}

void GlMesh::updateVerts(const float *vertices, const size_t vert_size,
                          const unsigned int *indices, const size_t ind_size,
                          const short *vertLogic, const size_t vert_logic_size,
                          const MeshTypes &meshType)
{
    glBindVertexArray(VAO); // Make sure the VAO is bound

    // Upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vert_size * sizeof(float), vertices, GL_STATIC_DRAW);

    // Upload index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind_size * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    // Set vertex attribute pointers
    int floatsPerVert = 0;
    for (size_t i = 0; i < vert_logic_size; i++)
        floatsPerVert += vertLogic[i];

    size_t numsSoFar = 0;
    for (size_t i = 0; i < vert_logic_size; i++)
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
    
    this->ind_size = ind_size;

    meshSize = calculateSizes(vertices, vert_size, indices, ind_size,vertLogic, vert_logic_size);
}

void GlMesh::Draw()
{
    shader->setImages(images);
    glBindVertexArray(VAO); // ngl who knows what this crap means, according to the names it applies and binds stuff
    glDrawElements(glMeshType, ind_size, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}