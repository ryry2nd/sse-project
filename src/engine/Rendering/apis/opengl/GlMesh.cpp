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

GlMesh::GlMesh(const Vert *verts, const size_t vert_size,
				const Ind *indices, const size_t ind_size
			) : VAO(0), VBO(0), EBO(0)
{
	spdlog::debug("Creating mesh with {} vertices, and {} indicies.", vert_size, ind_size);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	spdlog::debug("VAO has id: 0x{:x}", VAO);

	updateVerts(verts, vert_size, indices, ind_size);
}

void GlMesh::updateVerts(
	const Vert *verts, const size_t vert_size,
	const Ind *indices, const size_t ind_size
)
{
	glBindVertexArray(VAO);

	// ---- VBO ----
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER,
				 vert_size * sizeof(Vert),
				 verts,
				 GL_STATIC_DRAW);

	spdlog::debug("FBO has id: 0x{:x}", VBO);

	// ---- EBO (IMPORTANT: while VAO bound) ----
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				 ind_size * sizeof(Ind),
				 indices,
				 GL_STATIC_DRAW);

	spdlog::debug("EBO has id: 0x{:x}", EBO);

	// ---- ATTRIBUTES ----
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), (void*)offsetof(Vert, pos));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vert), (void*)offsetof(Vert, tex));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), (void*)offsetof(Vert, nor));
	glEnableVertexAttribArray(2);

	this->ind_size = ind_size;
	this->vert_size = vert_size;

	// IMPORTANT: unbind VAO LAST
	glBindVertexArray(0);
}