#pragma once

#include <SDL3/SDL_events.h>

namespace Engine::Scripting::Internal::Useable {
	long initModuleJIT(const char* path);
	void setupJIT(long id);
	bool loopJIT(long id);
	bool eventJIT(long id, SDL_Event *e);
	void shutdownJIT(long id);
	void removeJIT(long id);
	bool JITRunnable(long id);
	void (*getFunctionJIT(long id, const char *funcname))();

	long idTollvm(const char *);
}