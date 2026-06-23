#pragma once

#include "Core.hpp"

#include <string>
#include <vector>

namespace Engine::Rendering
{
	class Shader
	{
	public:
		Shader& operator=(const Shader&) = delete;
		Shader& operator=(Shader&&) noexcept = default;
		virtual ~Shader() = default;
	protected:
		static std::pair<std::vector<Uint32>, std::vector<Uint32>> getRaw(std::string path);
	};
}