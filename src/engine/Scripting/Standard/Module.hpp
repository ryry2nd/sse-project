#pragma once

extern "C" {
	const char *hostGetExe();
	const char *hostGetCurrent();
}

namespace Engine::Module {
	inline const char *getExe() {
		return hostGetExe();
	}
	inline const char *getCurrent() {
		return hostGetCurrent();
	}
}