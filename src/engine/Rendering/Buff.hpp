#pragma once

#include <cstddef>

namespace Engine::Rendering
{
	class Buff {
	public:
		enum class Type {
			Uniform,
			Storage,
		};
		enum class Frequency {
			Static,
			Dynamic,
			Stream,
		};

		virtual ~Buff() = default;
		Buff& operator=(const Buff&) = delete;
		Buff& operator=(Buff&&) noexcept = default;

		std::size_t getSize() {return allocSize;}

		virtual void write(const std::size_t offset, const std::size_t size, const void* data) = 0;
		virtual void read(const std::size_t offset, const std::size_t size, void* data) = 0;
		virtual void copy(const Buff *src, std::size_t sourceOffset, std::size_t destinationOffset, std::size_t size) = 0;

	protected:
		std::size_t allocSize;
	};
}