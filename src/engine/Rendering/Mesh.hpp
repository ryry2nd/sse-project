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
		enum class MeshTypes
		{
			Points,
			Lines,
			Triangles
		};

		virtual ~Mesh() = default;

		Mesh& operator=(const Mesh&) = delete;
		Mesh& operator=(Mesh&&) noexcept = default;

		MeshTypes getMeshType() {return meshType;}
		void setMeshType(MeshTypes meshType) {this->meshType = meshType;}

		bool getBackFaceCulling() {return backFaceCulling;}
		void toggleBackFaceCulling() {backFaceCulling = !backFaceCulling;}
	protected:
		MeshTypes meshType;
		bool backFaceCulling = true;
	};
}