#pragma once

namespace Engine::Scripting::Internal::Unusable {
	long initModuleJITMax(const char* path);
	void setupJITMax(long id);
	bool loopJITMax(long id);
	bool eventJITMax(long id, void *e);
	void shutdownJITMax(long id);
	void removeJITMax(long id);
	bool JITRunnableMax(long id);
	void (*getFunctionJITMax(long id, const char *funcname))();

	long initModuleJITMin(const char* path);
	void setupJITMin(long id);
	bool loopJITMin(long id);
	bool eventJITMin(long id, void *e);
	void shutdownJITMin(long id);
	void removeJITMin(long id);
	bool JITRunnableMin(long id);
	void (*getFunctionJITMin(long id, const char *funcname))();
}