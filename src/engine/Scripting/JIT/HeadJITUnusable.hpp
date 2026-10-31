#pragma once

namespace Engine::Scripting::Internal::Unusable {
	bool initModuleJITMax(const char* path);
	void removeJITMax();
	void *getFunctionJITMax(const char *funcname);

	bool initModuleJITMin(const char* path);
	void removeJITMin();
	void *getFunctionJITMin(const char *funcname);
}