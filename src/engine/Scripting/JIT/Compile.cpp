#include <SDL3/SDL_events.h>

#ifndef MINIMAL
#include <vector>
#include <cstring>
std::vector<long> minlist;
#endif

extern "C" {
#ifndef MINIMAL
long initModuleJITMax(const char* path);
void setupJITMax(long id);
bool loopJITMax(long id);
bool eventJITMax(long id, SDL_Event *e);
void shutdownJITMax(long id);
void removeJITMax(long id);
bool JITRunnableMax(long id);
#endif

long initModuleJITMin(const char* path);
void setupJITMin(long id);
bool loopJITMin(long id);
bool eventJITMin(long id, SDL_Event *e);
void shutdownJITMin(long id);
void removeJITMin(long id);
bool JITRunnableMin(long id);



long initModuleJIT(const char* path) {
	#ifdef MINIMAL
	return initModuleJITMin(path);
	#else
	const char* ext = std::strrchr(path, '.');
	if (ext && std::strcmp(ext, ".so") == 0) {
		long ret = initModuleJITMin(path);
		if (ret == -1) return -1;
		minlist.push_back(ret);
		return ret;
	}
	return initModuleJITMax(path);
	#endif
}
void setupJIT(long id) {
	#ifdef MINIMAL
	setupJITMin(id);
	#else
	if (std::ranges::find(minlist, id) != minlist.end()) {
		setupJITMin(id);
		return;
	}
	setupJITMax(id);
	#endif
}
bool loopJIT(long id) {
	#ifdef MINIMAL
	return loopJITMin(id);
	#else
	if (std::ranges::find(minlist, id) != minlist.end()) {
		return loopJITMin(id);
	}
	return loopJITMax(id);
	#endif
}
bool eventJIT(long id, SDL_Event *e) {
	#ifdef MINIMAL
	return eventJITMin(id, e);
	#else
	if (std::ranges::find(minlist, id) != minlist.end()) {
		return eventJITMin(id, e);
	}
	return eventJITMax(id, e);
	#endif
}
void shutdownJIT(long id) {
	#ifdef MINIMAL
	shutdownJITMin(id);
	#else
	if (std::ranges::find(minlist, id) != minlist.end()) {
		shutdownJITMin(id);
		return;
	}
	shutdownJITMax(id);
	#endif
}
void removeJIT(long id) {
	#ifdef MINIMAL
	removeJITMin(id);
	#else
	auto pos = std::ranges::find(minlist, id);
	if (pos != minlist.end()) {
		removeJITMin(id);
		minlist.erase(pos);
		return;
	}
	removeJITMax(id);
	#endif
}
bool JITRunnable(long id) {
	#ifdef MINIMAL
	return JITRunnableMin(id);
	#else
	if (std::ranges::find(minlist, id) != minlist.end()) {
		return JITRunnableMin(id);
	}
	return JITRunnableMax(id);
	#endif
}
}