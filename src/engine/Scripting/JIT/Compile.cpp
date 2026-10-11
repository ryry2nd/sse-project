// #define MINIMAL

#include "HeadJIT.hpp"
#include "HeadJITUnusable.hpp"

#include <SDL3/SDL_events.h>

#ifndef MINIMAL
#include <cstring>
#include <unordered_map>
std::unordered_map<long, bool> isMin;
#endif

using namespace Engine::Scripting::Internal::Unusable;
using namespace Engine::Scripting::Internal;

long Useable::initModuleJIT(const char* path) {
	#ifdef MINIMAL
	return initModuleJITMin(path);
	#else
	const char* ext = std::strrchr(path, '.');
	if (ext && std::strcmp(ext, ".so") == 0) {
		long ret = initModuleJITMin(path);
		if (ret == -1) return -1;
		isMin[ret] = true;
		return ret;
	}
	long ret = initModuleJITMax(path);
	isMin[ret] = false;
	return ret;
	#endif
}
void Useable::setupJIT(long id) {
	#ifdef MINIMAL
	setupJITMin(id);
	#else
	if (isMin[id]) {
		setupJITMin(id);
		return;
	}
	setupJITMax(id);
	#endif
}
void (*Useable::getFunctionJIT(long id, const char *funcname))() {
	#ifdef MINIMAL
	return getFunctionJITMin(id, funcname);
	#else
	if (isMin[id]) {
		return getFunctionJITMin(id, funcname);
	}
	return getFunctionJITMax(id, funcname);
	#endif
}
bool Useable::loopJIT(long id) {
	#ifdef MINIMAL
	return loopJITMin(id);
	#else
	if (isMin[id]) {
		return loopJITMin(id);
	}
	return loopJITMax(id);
	#endif
}
bool Useable::eventJIT(long id, SDL_Event *e) {
	#ifdef MINIMAL
	return eventJITMin(id, e);
	#else
	if (isMin[id]) {
		return eventJITMin(id, e);
	}
	return eventJITMax(id, e);
	#endif
}
void Useable::shutdownJIT(long id) {
	#ifdef MINIMAL
	shutdownJITMin(id);
	#else
	if (isMin[id]) {
		shutdownJITMin(id);
		return;
	}
	shutdownJITMax(id);
	#endif
}
void Useable::removeJIT(long id) {
	#ifdef MINIMAL
	removeJITMin(id);
	#else
	auto pos = isMin.find(id);
	if (pos != isMin.end()) {
		removeJITMin(id);
		isMin.erase(pos);
		return;
	}
	removeJITMax(id);
	#endif
}
bool Useable::JITRunnable(long id) {
	#ifdef MINIMAL
	return JITRunnableMin(id);
	#else
	if (isMin.find(id) != isMin.end()) {
		return JITRunnableMin(id);
	}
	return JITRunnableMax(id);
	#endif
}