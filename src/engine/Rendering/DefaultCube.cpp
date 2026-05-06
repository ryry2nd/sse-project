#include "Objects.hpp"


namespace Objects {
	const float cubeVertices[] = {
	// Positions          // Tex Coords (U, flipped V) // Normals
	// Front face
	-0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0, 0, 1,
	0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0, 0, 1,
	-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0, 0, 1,
	0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0, 0, 1,

	// Back face
	0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0, 0, -1,
	-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0, 0, -1,
	0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0, 0, -1,
	-0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0, 0, -1,

	// Left face
	-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -1, 0, 0,
	-0.5f, -0.5f, 0.5f, 1.0f, 1.0f, -1, 0, 0,
	-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1, 0, 0,
	-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -1, 0, 0,

	// Right face
	0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1, 0, 0,
	0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1, 0, 0,
	0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1, 0, 0,
	0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 1, 0, 0,

	// Top face
	-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0, 1, 0,
	0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0, 1, 0,
	-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0, 1, 0,
	0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0, 1, 0,

	// Bottom face
	-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0, -1, 0,
	0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0, -1, 0,
	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0, -1, 0,
	0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0, -1, 0
	};

	const unsigned int cubeIndices[] = {
	// Front face
	0, 1, 2,
	1, 3, 2,

	// Back face
	4, 5, 6,
	5, 7, 6,

	// Left face
	8, 9, 10,
	9, 11, 10,

	// Right face
	12, 13, 14,
	13, 15, 14,

	// Top face
	16, 17, 18,
	17, 19, 18,

	// Bottom face
	20, 21, 22,
	21, 23, 22
	};

	const float vertices2d[] = {
		//  x,     y,    u,   v
		-0.5f, -0.5f, 0.0f, 1.0f, // bottom-left
		0.5f, -0.5f, 1.0f, 1.0f,  // bottom-right
		0.5f, 0.5f, 1.0f, 0.0f,   // top-right
		-0.5f, 0.5f, 0.0f, 0.0f   // top-left
	};

	const unsigned int indices2d[] = {
		0, 1, 2,
		2, 3, 0
	};

	const size_t vertCount = sizeof(cubeVertices) / sizeof(cubeVertices[0]);
	const size_t indexCount = sizeof(cubeIndices) / sizeof(cubeIndices[0]);

	const size_t vert2dCount = sizeof(vertices2d) / sizeof(vertices2d[0]);
	const size_t ind2dCount = sizeof(indices2d) / sizeof(indices2d[0]);
}