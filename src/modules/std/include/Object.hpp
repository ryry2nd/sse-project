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

		Object(const Object&) = delete;
		Object& operator=(const Object&) = delete;

		Object(Object&& o) noexcept
			: mat(std::move(o.mat)), mesh(std::move(o.mesh)) {
			o.mat.clear();
			o.mesh.clear();
		}

		Object& operator=(Object&& o) noexcept {
			if (this != &o) {
				clear();
				mat = std::move(o.mat);
				mesh = std::move(o.mesh);
				o.mat.clear();
				o.mesh.clear();
			}
			return *this;
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