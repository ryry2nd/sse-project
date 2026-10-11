#pragma once

#include <SDL3/SDL_events.h>

#define MINIMAL

namespace Engine::Scripting::Internal::Unusable {
	#ifndef MINIMAL
	long initModuleJITMax(const char* path);
	void setupJITMax(long id);
	bool loopJITMax(long id);
	bool eventJITMax(long id, SDL_Event *e);
	void shutdownJITMax(long id);
	void removeJITMax(long id);
	bool JITRunnableMax(long id);
	void (*getFunctionJITMax(long id, const char *funcname))();
	#endif

	long initModuleJITMin(const char* path);
	void setupJITMin(long id);
	bool loopJITMin(long id);
	bool eventJITMin(long id, SDL_Event *e);
	void shutdownJITMin(long id);
	void removeJITMin(long id);
	bool JITRunnableMin(long id);
	void (*getFunctionJITMin(long id, const char *funcname))();
}