#pragma once

namespace Engine::Module {
	extern "C" {
		const char *hostGetExe();
		const char *hostGetCurrent();
		void (*hostGetFunction(const char *libname, const char *funcname))();
	}

	inline const char *getExe() {
		return hostGetExe();
	}
	inline const char *getCurrent() {
		return hostGetCurrent();
	}
	inline void (*getFunction(const char *libname, const char *funcname))() {
		return hostGetFunction(libname, funcname);
	}
}