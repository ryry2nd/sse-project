#include <engine/Rendering/Mesh.hpp>

using namespace Engine::Rendering;

inline void face(
    glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d,
    glm::vec3 n,
    glm::vec2 uvA, glm::vec2 uvB,
    glm::vec2 uvC, glm::vec2 uvD,
    std::vector<Vert>& vertices,
    std::vector<Ind>& indices)
{
    Ind start = (Ind)vertices.size();

    vertices.push_back({a, uvA, n});
    vertices.push_back({b, uvB, n});
    vertices.push_back({c, uvC, n});
    vertices.push_back({d, uvD, n});

    indices.insert(indices.end(), {
        start + 0, start + 1, start + 2,
        start + 2, start + 3, start + 0
    });
}

extern "C" {
    void buildVoxelGrid(
        std::vector<Vert>& vertices,
        std::vector<Ind>& indices,
        glm::vec3 size)
    {
        glm::vec3 p(0, 0, 0);
        glm::vec3 s = size;

        // +X: UV axes Y and Z
        face(
            p + glm::vec3(s.x, 0, 0),
            p + glm::vec3(s.x, s.y, 0),
            p + glm::vec3(s.x, s.y, s.z),
            p + glm::vec3(s.x, 0, s.z),
            {1, 0, 0},
            {s.z, s.y}, {s.z, 0}, {0, 0}, {0, s.y},
            vertices, indices
        );

        // -X
        face(
            p,
            p + glm::vec3(0, 0, s.z),
            p + glm::vec3(0, s.y, s.z),
            p + glm::vec3(0, s.y, 0),
            {-1, 0, 0},
            {0, s.y}, {s.z, s.y}, {s.z, 0}, {0, 0},
            vertices, indices
        );

        // +Y: UV axes X and Z
        face(
            p + glm::vec3(0, s.y, 0),
            p + glm::vec3(0, s.y, s.z),
            p + glm::vec3(s.x, s.y, s.z),
            p + glm::vec3(s.x, s.y, 0),
            {0, 1, 0},
            {0, 0}, {0, s.z}, {s.x, s.z}, {s.x, 0},
            vertices, indices
        );

        // -Y
        face(
            p,
            p + glm::vec3(s.x, 0, 0),
            p + glm::vec3(s.x, 0, s.z),
            p + glm::vec3(0, 0, s.z),
            {0, -1, 0},
            {0, s.z}, {s.x, s.z}, {s.x, 0}, {0, 0},
            vertices, indices
        );

        // +Z: UV axes X and Y
        face(
            p + glm::vec3(0, 0, s.z),
            p + glm::vec3(s.x, 0, s.z),
            p + glm::vec3(s.x, s.y, s.z),
            p + glm::vec3(0, s.y, s.z),
            {0, 0, 1},
            {0, s.y}, {s.x, s.y}, {s.x, 0}, {0, 0},
            vertices, indices
        );

        // -Z
        face(
            p,
            p + glm::vec3(0, s.y, 0),
            p + glm::vec3(s.x, s.y, 0),
            p + glm::vec3(s.x, 0, 0),
            {0, 0, -1},
            {s.x, s.y}, {s.x, 0}, {0, 0}, {0, s.y},
            vertices, indices
        );
    }

	void build2DFace(
		std::vector<Vert>& vertices,
		std::vector<Ind>& indices,
		glm::vec2 size,
		glm::vec3 position,
		glm::vec2 uvMin,
		glm::vec2 uvMax)
	{
		float w = size.x;
		float h = size.y;

		face(
			position,
			position + glm::vec3(w, 0, 0),
			position + glm::vec3(w, h, 0),
			position + glm::vec3(0, h, 0),
			{0, 0, 1},
			{uvMin.x, uvMax.y},
			{uvMax.x, uvMax.y},
			{uvMax.x, uvMin.y},
			{uvMin.x, uvMin.y},
			vertices,
			indices
		);
	}
}