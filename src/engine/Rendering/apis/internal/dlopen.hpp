#pragma once

#include <string>

#include <string>

#if defined(_WIN32)
#define EXE_EXT ".exe"
#define BIN_PREFIX ""
#define LIBRARY_SUFFIX ".dll"
#include <windows.h>

inline void *initruntime(const char *path, const char *& error, bool global=true) {
	if (!path) return nullptr;
	std::string libraryPath = std::string(BIN_PREFIX) + path;
	auto ret = LoadLibraryA(libraryPath.c_str());

	if (ret == nullptr) {
		DWORD code = GetLastError();
		static char buffer[512];
		FormatMessageA(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr,
			code,
			0,
			buffer,
			sizeof(buffer),
			nullptr
		);
		error = buffer;
	}

	return reinterpret_cast<void*>(ret);
}

inline void *getFunction(void* handle, const char* funcname) {
	if (!handle || !funcname) return nullptr;
    return GetProcAddress(reinterpret_cast<HMODULE>(handle), funcname);
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

inline void *initruntime(const char *path, const char *& error, bool global=true) {
	if (!path) return nullptr;
	std::string libraryPath = std::string(BIN_PREFIX) + path;
	auto ret = dlopen(libraryPath.c_str(), RTLD_NOW | (global ? RTLD_GLOBAL : RTLD_LOCAL));

	if (ret == nullptr) {
		error = dlerror();
	}

	return ret;
}
inline void *getFunction(void *handle, const char *funcname) {
	if (!handle || !funcname) return nullptr;

	return dlsym(handle, funcname);
}
inline int rmruntime(void *handle) {
	if (!handle) return 0;
	return dlclose(handle);
}
#else
#error "Unsupported platform: runtime loading is not implemented"
#endif