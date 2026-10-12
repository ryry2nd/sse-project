#include <filesystem>
#include <unordered_map>
#include <spdlog/spdlog.h>

#include "../../Rendering/apis/internal/dlopen.hpp"

struct MyModule {
	void *runtime;
	void (*setup)();
	void (*loop)();
	void (*event)(void*);
	void (*shutdown)();
};

std::unordered_map<long, MyModule> modsMin;


#include "HeadJITUnusable.hpp"

using namespace Engine::Scripting::Internal;

long Unusable::initModuleJITMin(const char* path) {
	static long currID = 0;
	std::filesystem::path file = path;

	if (file.extension() != LIBRARY_SUFFIX) return -1;

	void *handle = initruntime(path);

	if (!handle)
	{
		spdlog::error("Failed to load {}", file.string());
		return -1;
	}

	MyModule mod;

	auto setup = getFunction(handle, "setup");
	auto loop = getFunction(handle, "loop");
	auto event = getFunction(handle, "event");
	auto shutdown = getFunction(handle, "shutdown");


	mod.runtime = handle;
	mod.setup = reinterpret_cast<void(*)()>(setup);
	mod.loop = reinterpret_cast<void(*)()>(loop);
	mod.event = reinterpret_cast<void(*)(void*)>(event);
	mod.shutdown = reinterpret_cast<void(*)()>(shutdown);

	modsMin[currID] = std::move(mod);
	return currID++;
}
void Unusable::setupJITMin(long id) {
	auto it = modsMin.find(id);
	if (it == modsMin.end()) return;

	if (!modsMin[id].setup) return;

	modsMin[id].setup();
}
bool Unusable::loopJITMin(long id) {
	auto it = modsMin.find(id);
	if (it == modsMin.end()) return 1;

	if (!modsMin[id].loop)	return 1;

	modsMin[id].loop();
	return 0;
}
bool Unusable::eventJITMin(long id, void *e) {
	auto it = modsMin.find(id);
	if (it == modsMin.end()) return 1;

	if (!modsMin[id].event) {
		return 1;
	}
	modsMin[id].event(e);
	return 0;
}
void Unusable::shutdownJITMin(long id) {
	auto it = modsMin.find(id);
	if (it == modsMin.end()) return;

	if (!modsMin[id].shutdown) {
		return;
	}
	modsMin[id].shutdown();
}
void Unusable::removeJITMin(long id) {
	auto it = modsMin.find(id);
	if (it == modsMin.end()) return;

	modsMin[id].setup = nullptr;
	modsMin[id].loop = nullptr;
	modsMin[id].event = nullptr;
	modsMin[id].shutdown = nullptr;
	rmruntime(modsMin[id].runtime);
	modsMin.erase(id);
}

bool Unusable::JITRunnableMin(long id) {
	auto it = modsMin.find(id);
	if (it == modsMin.end()) return 0;

	return !(modsMin[id].loop || modsMin[id].event);
}

void (*Unusable::getFunctionJITMin(long id, const char *funcname))() {
	return getFunction(modsMin[id].runtime, funcname);
}