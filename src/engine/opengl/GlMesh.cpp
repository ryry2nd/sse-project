#include "HelperFunctionsOpengl.hpp"

OpenGlMesh::~OpenGlMesh()
{
    if (VAO != 0)
        glDeleteVertexArrays(1, &VAO);
    if (VBO != 0)
        glDeleteBuffers(1, &VBO);
}

void OpenGlMesh::setupObject(const std::vector<float> &vertices, const std::vector<short> &vertLogic, const MeshTypes &meshType)
{
    if (VAO != 0)
        glDeleteVertexArrays(1, &VAO);
    if (VBO != 0)
        glDeleteBuffers(1, &VBO);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    updateVerts(vertices, vertLogic, meshType);
}

void OpenGlMesh::updateVerts(const std::vector<float> &vertices, const std::vector<short> &vertLogic, const MeshTypes &meshType)
{
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    float floatsPerVert = 0;

    for (short s : vertLogic)
    {
        floatsPerVert += s;
    }

    size = vertices.size() / floatsPerVert;

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

void OpenGlMesh::finalizeShaders()
{
    glBindVertexArray(VAO); // ngl who knows what this crap means, according to the names it applies and binds stuff
    glDrawArrays(glMeshType, 0, size);
    glBindVertexArray(0);
}

Mesh *OpenGlMesh::makeNewMesh() const
{
    return new OpenGlMesh();
}