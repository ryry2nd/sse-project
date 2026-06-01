#include "GlRendering.hpp"
#include <spdlog/spdlog.h>

using namespace OpenGl;
using namespace Engine::Rendering;

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
				Mesh::MeshTypes meshType
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
	Mesh::MeshTypes meshType
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

	this->glMeshType = convertMeshType(meshType);
	this->meshType = meshType;
	this->ind_size = ind_size;
	this->vert_size = vert_size;

	// IMPORTANT: unbind VAO LAST
	glBindVertexArray(0);
}