#pragma once

#include <cstddef>

namespace Engine::Rendering::Mesh {
	enum class MeshTypes
	{
		Points,
		Lines,
		Triangles
	};
}

extern "C" {
	void hostCreateMesh(const char *name, const float *vertices, const size_t vert_size, const unsigned int *indices, const size_t ind_size, const short *vertLogic, const size_t vert_logic_size, Engine::Rendering::Mesh::MeshTypes meshType = Engine::Rendering::Mesh::MeshTypes::Triangles);
	void hostSetMesh(const char *name);
	Engine::Rendering::Mesh::MeshTypes hostGetMeshType();
	void hostSetMeshType(Engine::Rendering::Mesh::MeshTypes meshType);
	bool hostMeshGetBackFaceCulling();
	void hostMeshToggleBackFaceCulling();
	void hostRemoveMesh();
}

namespace Engine::Rendering::Mesh {
	inline void createMesh(const char *name, const float *vertices, const size_t vert_size, const unsigned int *indices, const size_t ind_size, const short *vertLogic, const size_t vert_logic_size, Engine::Rendering::Mesh::MeshTypes meshType = Engine::Rendering::Mesh::MeshTypes::Triangles) {
		hostCreateMesh(name, vertices, vert_size, indices, ind_size, vertLogic, vert_logic_size, meshType);
	}
	inline void removeMesh() {
		hostRemoveMesh();
	}
	inline void setMesh(const char *name) {
		hostSetMesh(name);
	}
	inline MeshTypes getMeshType() {
		return hostGetMeshType();
	}
	inline void setMeshType(Mesh::MeshTypes meshType) {
		hostSetMeshType(meshType);
	}
	inline bool getBackFaceCulling() {
		return hostMeshGetBackFaceCulling();
	}
	inline void toggleBackFaceCulling() {
		hostMeshToggleBackFaceCulling();
	}
}