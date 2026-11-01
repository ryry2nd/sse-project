#include "Rendering.hpp"

GlMesh::~GlMesh()
{
	SCOPE("Rendering::OpenGL::Mesh::destroy");
	if (VAO != 0)
		glDeleteVertexArrays(1, &VAO);


	VBO.reset();
	EBO.reset();
}

GlMesh::GlMesh(const Vert *verts, const size_t vert_size,
				const Ind *indices, const size_t ind_size
			)
{
	SCOPE("Rendering::OpenGL::Mesh::create");

	GLuint VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	// ---- VBO ----
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER,
				 vert_size * sizeof(Vert),
				 verts,
				 GL_STATIC_DRAW);

	// ---- EBO (IMPORTANT: while VAO bound) ----
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				 ind_size * sizeof(Ind),
				 indices,
				 GL_STATIC_DRAW);

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

	this->VBO = std::make_unique<GlBuff>(
		VBO, Buff::Type::Storage,
		vert_size * sizeof(Vert)
	);

	this->EBO = std::make_unique<GlBuff>(
		EBO, Buff::Type::Storage,
		ind_size * sizeof(Ind)
	);
}

Buff* GlMesh::vertBuff() {
    return VBO.get();
}

Buff* GlMesh::indBuff() {
    return EBO.get();
}

extern "C" std::unique_ptr<Mesh> createMesh(const Vert *vertices, const size_t vert_size, const Ind *indices, const size_t ind_size) {
    return std::make_unique<GlMesh>(vertices, vert_size, indices, ind_size);
}