#pragma once

#include <string>

namespace Engine::Module {
	extern "C" {
		const char *hostGetExe();
		const char *hostGetCurrentRunning();
		const char *hostGetID(const char *path);
		void *hostGetFunction(const char *libname, const char *funcname);
		const char *hostIdToPath(const char *id);
	}

	inline const char *getExe() {
		return hostGetExe();
	}
	inline const char *getCurrentRunning() {
		return hostGetCurrentRunning();
	}

	inline const char *idToPath(const char *id) {
		return hostIdToPath(id);
	}

	inline const char *getID(std::string path) {
		return hostGetID(path.c_str());
	}

	inline void *getFunction(const char *libname, const char *funcname) {
		return hostGetFunction(libname, funcname);
	}
}