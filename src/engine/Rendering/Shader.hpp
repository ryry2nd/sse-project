#pragma once

namespace Engine::Rendering
{
	class Shader
	{
	public:
		Shader& operator=(const Shader&) = delete;
		Shader& operator=(Shader&&) noexcept = default;

		virtual ~Shader() = default;
	};
}