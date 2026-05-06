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

	spdlog::debug("Deleting mesh with VAO: 0x{:x}, VBO: 0x{:x}, EBO: 0x{:x}", VAO, VBO, EBO);
}

GlMesh::GlMesh(const float *vertices, const size_t vert_size,
				const unsigned int *indices, const size_t ind_size,
				const short *vertLogic, const size_t vert_logic_size,
				Rendering::Mesh::MeshTypes meshType
			) : VAO(0), VBO(0), EBO(0)
{
	spdlog::debug("Creating mesh with {} vertices, and {} indicies.", vert_size, ind_size);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	spdlog::debug("VAO has id: 0x{:x}", VAO);

	updateVerts(vertices, vert_size, indices, ind_size, vertLogic, vert_logic_size, meshType);
}

void GlMesh::updateVerts(
	const float *vertices, const size_t vert_size,
	const unsigned int *indices, const size_t ind_size,
	const short *vertLogic, const size_t vert_logic_size,
	Rendering::Mesh::MeshTypes meshType
)
{
	glBindVertexArray(VAO);

	// ---- VBO ----
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER,
				 vert_size * sizeof(float),
				 vertices,
				 GL_STATIC_DRAW);

	spdlog::debug("FBO has id: 0x{:x}", VBO);

	// ---- EBO (IMPORTANT: while VAO bound) ----
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				 ind_size * sizeof(unsigned int),
				 indices,
				 GL_STATIC_DRAW);

	spdlog::debug("EBO has id: 0x{:x}", EBO);

	// ---- ATTRIBUTES ----
	GLuint offset = 0;

	int strideFloats = 0;
	for (size_t i = 0; i < vert_logic_size; i++)
		strideFloats += vertLogic[i];

	GLsizei stride = strideFloats * sizeof(float);

	for (size_t i = 0; i < vert_logic_size; i++)
	{
		glVertexAttribPointer(
			i,
			vertLogic[i],
			GL_FLOAT,
			GL_FALSE,
			stride,
			(void*)(offset * sizeof(float))
		);

		glEnableVertexAttribArray(i);
		offset += vertLogic[i];
	}

	// ---- TYPE ----
	switch (meshType)
	{
		case MeshTypes::Points:     glMeshType = GL_POINTS; break;
		case MeshTypes::Lines:      glMeshType = GL_LINES; break;
		case MeshTypes::Triangles:  glMeshType = GL_TRIANGLES; break;
	}

	this->meshType = meshType;
	this->ind_size = ind_size;
	this->vert_size = vert_size;

	meshSize = calculateSizes(vertices, vert_size, indices, ind_size,
							  vertLogic, vert_logic_size);

	// IMPORTANT: unbind VAO LAST
	glBindVertexArray(0);
}

glm::vec3 GlMesh::getMeshSize() {
	return meshSize;
}
GLuint GlMesh::getVAO() {
	return VAO;
}
GLuint GlMesh::getVBO() {
	return VBO;
}
GLuint GlMesh::getEBO() {
	return EBO;
}
GLenum GlMesh::getMeshType() {
	return glMeshType;
}
GLsizei GlMesh::getSize() {
	return size;
}

size_t GlMesh::getInd() {
	return ind_size;
}

size_t GlMesh::getVert() {
	return vert_size;
}