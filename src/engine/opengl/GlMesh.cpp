#include "HelperFunctionsOpengl.hpp"

OpenGlMesh::~OpenGlMesh()
{
    if (VAO != 0)
        glDeleteVertexArrays(1, &VAO);
    if (VBO != 0)
        glDeleteBuffers(1, &VBO);
}

OpenGlMesh::OpenGlMesh(const std::vector<float> &vertices, const std::vector<short> &vertLogic, const MeshTypes &meshType) : VAO(0), VBO(0)
{
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

    this->vertices = vertices;
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

void OpenGlMesh::finalizeShaders()
{
    glBindVertexArray(VAO); // ngl who knows what this crap means, according to the names it applies and binds stuff
    glDrawArrays(glMeshType, 0, size);
    glBindVertexArray(0);
}

glm::vec3 OpenGlMesh::calculateSizes()
{
    glm::vec3 minCorner(FLT_MAX);
    glm::vec3 maxCorner(-FLT_MAX);

    int total = 0;
    for (short &i : vertLogic)
        total += i;

    for (size_t i = 0; i < vertices.size(); i += total)
    {
        glm::vec3 pos(vertices[i], vertices[i + 1], vertices[i + 2]);
        minCorner = glm::min(minCorner, pos);
        maxCorner = glm::max(maxCorner, pos);
    }

    return maxCorner - minCorner;
}

Mesh *OpenGlMesh::makeNewMesh(const std::vector<float> &vertices, const std::vector<short> &vertLogic, const MeshTypes &meshType) const
{
    return new OpenGlMesh(vertices, vertLogic, meshType);
}

Mesh *OpenGlMesh::makeCopy() const
{
    OpenGlMesh *ret = new OpenGlMesh(vertices, vertLogic, meshType);
    ret->sizeOffset = sizeOffset;
    ret->posOffset = posOffset;
    ret->rotOffset = rotOffset;
    return ret;
}