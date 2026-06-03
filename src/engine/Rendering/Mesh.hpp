#pragma once

#include <glm/glm.hpp>

namespace Engine::Rendering
{
	struct Vert {
		glm::vec3 pos;
		glm::vec2 tex;
		glm::vec3 nor;
	};

	typedef unsigned int Ind;

	class Mesh
	{
	public:
		virtual ~Mesh() = default;

		Mesh& operator=(const Mesh&) = delete;
		Mesh& operator=(Mesh&&) noexcept = default;
	};
}