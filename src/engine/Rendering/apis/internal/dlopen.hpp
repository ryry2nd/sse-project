#pragma once

#include <string>

#if defined(_WIN32)
#define EXE_EXT ".exe"
#define BIN_PREFIX ""
#define LIBRARY_SUFFIX ".dll"
#include <windows.h>

inline void* initruntime(const char* path) {
	if (!path) return nullptr;
    std::string libraryPath = std::string(BIN_PREFIX) + path;
    return reinterpret_cast<void*>(LoadLibraryA(libraryPath.c_str()));
}

inline void (*getFunction(void* handle, const char* funcname))() {
	if (!handle || !funcname) return nullptr;
    return reinterpret_cast<void (*)()>(
        GetProcAddress(reinterpret_cast<HMODULE>(handle), funcname)
    );
}

inline int rmruntime(void* handle) {
	if (!handle) return 0;
    return FreeLibrary(reinterpret_cast<HMODULE>(handle)) ? 0 : -1;
}
#elif defined(__linux__) || defined(__ANDROID__) || (defined(__APPLE__) && defined(__MACH__))

#define EXE_EXT ""
#define BIN_PREFIX "./"
#if (defined(__APPLE__) && defined(__MACH__))
#define LIBRARY_SUFFIX ".dylib"
#else
#define LIBRARY_SUFFIX ".so"
#endif
#include <dlfcn.h>

inline void *initruntime(const char *path) {
	if (!path) return nullptr;
	std::string libraryPath = std::string(BIN_PREFIX) + path;
	return dlopen(libraryPath.c_str(), RTLD_NOW | RTLD_GLOBAL);
}
inline void (*getFunction(void *handle, const char *funcname))() {
	if (!handle || !funcname) return nullptr;
	auto function = reinterpret_cast<void (*)()>(
		dlsym(handle, funcname)
	);

	return function;
}
inline int rmruntime(void *handle) {
	if (!handle) return 0;
	return dlclose(handle);
}
#else
#error "Unsupported platform: runtime loading is not implemented"
#endif