#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace Engine::Rendering
{
	struct Vert {
		glm::vec3 pos;
		glm::vec2 tex;
		glm::vec3 nor;
	};

	typedef unsigned int Ind;

	struct Shape {
		std::vector<Vert> verts;
		std::vector<Ind> inds;

		// Colors
		glm::vec3 ambient  = {0.0f, 0.0f, 0.0f}; // Ka
		glm::vec3 diffuse  = {1.0f, 1.0f, 1.0f}; // Kd
		glm::vec3 specular = {0.0f, 0.0f, 0.0f}; // Ks
		glm::vec3 emissive = {0.0f, 0.0f, 0.0f}; // Ke

		// Surface properties
		float shininess = 1.0f; // Ns
		float opacity   = 1.0f; // d / Tr
		float ior       = 1.0f; // Ni

		// Textures
		std::string ambientTex;   // map_Ka
		std::string diffuseTex;   // map_Kd
		std::string specularTex;  // map_Ks
		std::string emissiveTex;  // map_Ke
		std::string shininessTex; // map_Ns
		std::string opacityTex;   // map_d
		std::string bumpTex;      // bump / map_Bump
		std::string normalTex;    // norm
		std::string displacementTex; // disp
	};

	class Mesh
	{
	public:
		virtual ~Mesh() = default;

		Mesh& operator=(const Mesh&) = delete;
		Mesh& operator=(Mesh&&) noexcept = default;

		static void getObj(const char *fname, std::vector<Shape> &shapesout);
	};
}