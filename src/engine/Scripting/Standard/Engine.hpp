#pragma once

#include "Logging.hpp"
#include "Helper.hpp"
#include <engine/Rendering/Rendering.hpp>

extern "C" {
	void engineShutdown();
}

namespace Engine {
	inline void shutdown() {
		engineShutdown();
	}
}