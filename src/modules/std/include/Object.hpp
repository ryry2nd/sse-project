#pragma once

#include <engine/Engine.hpp>
#include <glm/glm.hpp>
#include <vector>

namespace ObjectSpace {
	using namespace Engine::Rendering;

	class Object {
	public:
		std::vector<Material> mat;
		std::vector<Mesh *> mesh;

		Object() {
			mat = std::vector<Material>(1);
			mesh = std::vector<Mesh *>(1);
		}

		void draw(RenderTarget *target) {
			if (mat.empty()) return;
			if (mesh.empty()) return;
			for (int i = 0; i < mesh.size(); i++) {
				Engine::Rendering::draw(target, mesh[i], &mat[i % mesh.size()]);
			}
		}
	};
}