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

		Buff(const Buff&) = delete;
		Buff& operator=(const Buff&) = delete;
		Buff(Buff&&) noexcept = default;
		Buff& operator=(Buff&&) noexcept = default;

		std::size_t getSize() {return allocSize;}
		Type getType() {return buffType;}
		Frequency getFrequency() {return freq;}

		virtual void write(const std::size_t offset, const std::size_t size, const void* data) = 0;
		virtual void read(const std::size_t offset, const std::size_t size, void* data) = 0;

	protected:
		Buff(Type type, Frequency freq, std::size_t size) {
			this->buffType = type;
			this->freq = freq;
			this->allocSize = size;
		}
		Type buffType;
		Frequency freq;
		std::size_t allocSize;
	};
}