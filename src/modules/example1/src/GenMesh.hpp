#pragma once

#include <engine/Rendering/Mesh.hpp>

using namespace Engine::Rendering;


void buildVoxelGrid(std::vector<Vert>& vertices,
                    std::vector<Ind>& indices,
                    glm::vec3 size)
{
    auto isSolid = [&](int x, int y, int z) -> bool
    {
        if (x < 0 || y < 0 || z < 0 ||
            x >= size.x || y >= size.y || z >= size.z)
            return false;

        return true; // replace later with real voxel data
    };

    auto face = [&](glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, glm::vec3 n)
    {
        Ind start = (Ind)vertices.size();

        vertices.push_back({a, {0,0}, n});
        vertices.push_back({b, {1,0}, n});
        vertices.push_back({c, {1,1}, n});
        vertices.push_back({d, {0,1}, n});

        indices.insert(indices.end(), {
            start+0, start+1, start+2,
            start+2, start+3, start+0
        });
    };

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
			{1,0,0}
		);

		if (!isSolid(x-1,y,z))
		face(
			p,
			p+glm::vec3(0,0,1),
			p+glm::vec3(0,1,1),
			p+glm::vec3(0,1,0),
			{-1,0,0}
		);

		if (!isSolid(x,y+1,z))
		face(
			p+glm::vec3(0,1,0),
			p+glm::vec3(0,1,1),
			p+glm::vec3(1,1,1),
			p+glm::vec3(1,1,0),
			{0,1,0}
		);

		if (!isSolid(x,y-1,z))
		face(
			p,
			p+glm::vec3(1,0,0),
			p+glm::vec3(1,0,1),
			p+glm::vec3(0,0,1),
			{0,-1,0}
		);

		if (!isSolid(x,y,z+1))
		face(
			p+glm::vec3(0,0,1),
			p+glm::vec3(1,0,1),
			p+glm::vec3(1,1,1),
			p+glm::vec3(0,1,1),
			{0,0,1}
		);

		if (!isSolid(x,y,z-1))
		face(
			p,
			p+glm::vec3(0,1,0),
			p+glm::vec3(1,1,0),
			p+glm::vec3(1,0,0),
			{0,0,-1}
		);
    }
}