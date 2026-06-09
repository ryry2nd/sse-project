#pragma once

#include <engine/Rendering/Mesh.hpp>

using namespace Engine::Rendering;

#define isSolid(x, y, z) (x < 0 || y < 0 || z < 0 || x >= size.x || y >= size.y || z >= size.z)

#define face(a, b, c, d, n) \
{ \
	Ind start = (Ind)vertices.size(); \
	vertices.push_back({a, {0,0}, n}); \
	vertices.push_back({b, {1,0}, n}); \
	vertices.push_back({c, {1,1}, n}); \
	vertices.push_back({d, {0,1}, n}); \
	indices.push_back(start+0); \
	indices.push_back(start+1); \
	indices.push_back(start+2); \
	indices.push_back(start+2); \
	indices.push_back(start+3); \
	indices.push_back(start+0); \
}

void buildVoxelGrid(std::vector<Vert>& vertices,
                    std::vector<Ind>& indices,
                    glm::vec3 size)
{
	size_t surfaceApprox =
		2 * (
			size_t(size.x * size.y) +
			size_t(size.x * size.z) +
			size_t(size.y * size.z)
		);

	vertices.reserve(surfaceApprox * 4);
	indices.reserve(surfaceApprox * 6);

    for (int x = 0; x < size.x; x++)
    for (int y = 0; y < size.y; y++)
    for (int z = 0; z < size.z; z++)
    {
        if (!isSolid(x,y,z)) continue;

        glm::vec3 p(x,y,z);

        if (!isSolid(x+1,y,z))
		face(
			p+glm::vec3(1,0,0),
			p+glm::vec3(1,1,0),
			p+glm::vec3(1,1,1),
			p+glm::vec3(1,0,1),
			glm::vec3(1,0,0)
		);

		if (!isSolid(x-1,y,z))
		face(
			p,
			p+glm::vec3(0,0,1),
			p+glm::vec3(0,1,1),
			p+glm::vec3(0,1,0),
			glm::vec3(-1,0,0)
		);

		if (!isSolid(x,y+1,z))
		face(
			p+glm::vec3(0,1,0),
			p+glm::vec3(0,1,1),
			p+glm::vec3(1,1,1),
			p+glm::vec3(1,1,0),
			glm::vec3(0,1,0)
		);

		if (!isSolid(x,y-1,z))
		face(
			p,
			p+glm::vec3(1,0,0),
			p+glm::vec3(1,0,1),
			p+glm::vec3(0,0,1),
			glm::vec3(0,-1,0)
		);

		if (!isSolid(x,y,z+1))
		face(
			p+glm::vec3(0,0,1),
			p+glm::vec3(1,0,1),
			p+glm::vec3(1,1,1),
			p+glm::vec3(0,1,1),
			glm::vec3(0,0,1)
		);

		if (!isSolid(x,y,z-1))
		face(
			p,
			p+glm::vec3(0,1,0),
			p+glm::vec3(1,1,0),
			p+glm::vec3(1,0,0),
			glm::vec3(0,0,-1)
		);
    }
}