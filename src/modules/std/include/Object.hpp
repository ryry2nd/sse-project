#pragma once

#include <engine/Engine.hpp>
#include <glm/glm.hpp>

namespace ObjectSpace {
	using namespace Engine::Rendering;

	class Object {
	public:
		Material mat;
		Mesh *mesh;

		void draw(RenderTarget *target) {
			Engine::Rendering::draw(target, mesh, &mat);
		}
	};
}