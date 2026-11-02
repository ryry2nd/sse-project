#pragma once

#include <engine/Engine.hpp>
#include <glm/glm.hpp>
#include <vector>

namespace ObjectSpace {
	using namespace Engine::Rendering;

	class Object {
	public:
		std::vector<Material> mat{1};
		std::vector<Mesh *> mesh{1};

		void draw(RenderTarget *target) {
			for (int i = 0; i < mesh.size(); i++) {
				if (!mesh[i]) continue;
                if (mat.empty()) continue;
				Engine::Rendering::draw(target, mesh[i], &mat[i % mesh.size()]);
			}
		}
	};
}