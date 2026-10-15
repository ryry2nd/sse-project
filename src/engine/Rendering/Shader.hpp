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
		bool isCompute() {return bisCompute;}
	protected:
		static std::pair<std::vector<Uint32>, std::vector<Uint32>> getRaw(std::string path);
		static std::vector<Uint32> getRawCompute(std::string prePath);
		bool bisCompute = false;
	};
}