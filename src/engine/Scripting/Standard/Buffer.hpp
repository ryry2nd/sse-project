#pragma once

#include <cstddef>

namespace Engine::Rendering::Buff {
	enum class Type {
		Uniform,
		Storage,
	};
	enum class Frequency {
		Static,
		Dynamic,
		Stream,
	};
}

extern "C" {
	void hostCreateBuffer(const char *name, Engine::Rendering::Buff::Type type, Engine::Rendering::Buff::Frequency freq, std::size_t size, const void* data = nullptr);
	std::size_t hostBuffGetSize();
	Engine::Rendering::Buff::Type hostBuffGetType();
	Engine::Rendering::Buff::Frequency hostBuffGetFrequency();
	void hostBuffWrite(const std::size_t offset, const std::size_t size, const void* data);
	void hostBuffRead(const std::size_t offset, const std::size_t size, void* data);
	void hostSetBuffer(const char *name);
}

namespace Engine::Rendering::Buff {
	inline void createBuffer(const char *name, Engine::Rendering::Buff::Type type, Engine::Rendering::Buff::Frequency freq, std::size_t size, const void* data = nullptr) {
		hostCreateBuffer(name, type, freq, size, data);
	}
	inline std::size_t getSize() {
		return hostBuffGetSize();
	}
	inline Buff::Type getType() {
		return hostBuffGetType();
	}
	inline Buff::Frequency getFrequency() {
		return hostBuffGetFrequency();
	}
	inline void write(const std::size_t offset, const std::size_t size, const void* data) {
		hostBuffWrite(offset, size, data);
	}
	inline void read(const std::size_t offset, const std::size_t size, void* data) {
		hostBuffRead(offset, size, data);
	}
	inline void setBuffer(const char *name) {
		hostSetBuffer(name);
	}
}

