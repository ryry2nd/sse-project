#pragma once

#include "Window.hpp"

extern "C" {
	void hostShutDownAll();
}

namespace Engine::Rendering {
	inline void ShutdownAll() {
		hostShutDownAll();
	}
}