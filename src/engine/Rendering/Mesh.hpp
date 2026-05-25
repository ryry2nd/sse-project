#pragma once

namespace Engine::Rendering
{
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