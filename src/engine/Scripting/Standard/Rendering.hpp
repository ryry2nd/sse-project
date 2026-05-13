#pragma once

#include "Window.hpp"
#include "Buffer.hpp"
#include "FrameBuffer.hpp"
#include "Image.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include "Draw.hpp"

extern "C" {
	void hostShutDownAll();
}

namespace Engine::Rendering {
	inline void ShutdownAll() {
		hostShutDownAll();
	}
}