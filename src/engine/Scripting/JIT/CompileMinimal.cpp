#include <filesystem>
#include <spdlog/spdlog.h>

#include "../../Rendering/apis/internal/dlopen.hpp"

void *runtimeMin = nullptr;


#include "HeadJITUnusable.hpp"

using namespace Engine::Scripting::Internal;

bool Unusable::initModuleJITMin(const char* path) {
	if (runtimeMin) rmruntime(runtimeMin);

	std::filesystem::path file = path;

	if (file.extension() != LIBRARY_SUFFIX) return false;

	const char *err;
	runtimeMin = initruntime(path, err);

	if (!runtimeMin)
	{
		spdlog::error("Failed to load: {}", err);
		return false;
	}

	return true;
}
void Unusable::removeJITMin() {
	rmruntime(runtimeMin);
	runtimeMin = nullptr;
}

void *Unusable::getFunctionJITMin(const char *funcname) {
	return getFunction(runtimeMin, funcname);
}