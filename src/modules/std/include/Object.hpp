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

		~Object() {
			clear();
		}

		void clear() {
			for (int i = 0; i < mesh.size(); i++) {
				removeMesh(mesh[i]);
			}
			mesh.clear();

			for (int i = 0; i < mat.size(); i++) {
				removeShader(mat[i].shader);
				for (auto &im: mat[i].images)
					removeImage(im.second);
				for (auto &im: mat[i].rwimages)
					removeImage(im.second);
				for (auto &im: mat[i].ubo)
					removeBuff(im.second);
				for (auto &im: mat[i].ssbo)
					removeBuff(im.second);
			}
			mat.clear();

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