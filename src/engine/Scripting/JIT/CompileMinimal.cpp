#include <memory>
#include <filesystem>
#include <unordered_map>
#include <SDL3/SDL_loadso.h>
#include <SDL3/SDL_events.h>
#include <spdlog/spdlog.h>

#ifdef _WIN32
#define POPEN _popen
#define PCLOSE _pclose
#define EXE_EXT ".exe"
#define BIN_PREFIX ""
#define LIBRARY_SUFFIX ".dll"
#else
#define POPEN popen
#define PCLOSE pclose
#define EXE_EXT ""
#define BIN_PREFIX "./"
#define LIBRARY_SUFFIX ".so"
#endif


auto sdlDeleter = [](SDL_SharedObject* obj) {
	if (obj) SDL_UnloadObject(obj);
};

using SDLSharedPtr = std::unique_ptr<SDL_SharedObject, decltype(sdlDeleter)>;

struct MyModule {
	SDLSharedPtr runtime;
	void (*setup)();
	void (*loop)();
	void (*event)(SDL_Event*, bool*);
	void (*shutdown)();
};

std::unordered_map<long, MyModule> modsMin;


extern "C" {
long initModuleJITMin(const char* path) {
	static long currID = 0;
	std::filesystem::path file = path;

	if (file.extension() != LIBRARY_SUFFIX) return -1;

	SDL_SharedObject* raw = SDL_LoadObject(file.string().c_str());

	if (!raw)
	{
		spdlog::error("Failed to load {}: {}", file.string(), SDL_GetError());
		return -1;
	}

	SDLSharedPtr lib(raw, sdlDeleter);

	MyModule mod;

	auto setup = SDL_LoadFunction(lib.get(), "setup");
	auto loop = SDL_LoadFunction(lib.get(), "loop");
	auto event = SDL_LoadFunction(lib.get(), "event");
	auto shutdown = SDL_LoadFunction(lib.get(), "shutdown");


	mod.runtime = std::move(lib);
	mod.setup = reinterpret_cast<void(*)()>(setup);
	mod.loop = reinterpret_cast<void(*)()>(loop);
	mod.event = reinterpret_cast<void(*)(SDL_Event*, bool*)>(event);
	mod.shutdown = reinterpret_cast<void(*)()>(shutdown);

	modsMin[currID] = std::move(mod);
	return currID++;
}
void setupJITMin(long id) {
	auto it = modsMin.find(id);
	if (it == modsMin.end()) return;

	if (!modsMin[id].setup) return;

	modsMin[id].setup();
}
bool loopJITMin(long id) {
	auto it = modsMin.find(id);
	if (it == modsMin.end()) return 1;

	if (!modsMin[id].loop)	return 1;

	modsMin[id].loop();
	return 0;
}
bool eventJITMin(long id, SDL_Event *e, bool *running) {
	auto it = modsMin.find(id);
	if (it == modsMin.end()) return 1;

	if (!modsMin[id].event) {
		return 1;
	}
	modsMin[id].event(e, running);
	return 0;
}
void shutdownJITMin(long id) {
	auto it = modsMin.find(id);
	if (it == modsMin.end()) return;

	if (!modsMin[id].shutdown) {
		return;
	}
	modsMin[id].shutdown();
}
void removeJITMin(long id) {
	auto it = modsMin.find(id);
	if (it == modsMin.end()) return;

	modsMin[id].setup = nullptr;
	modsMin[id].loop = nullptr;
	modsMin[id].event = nullptr;
	modsMin[id].shutdown = nullptr;
	modsMin[id].runtime.reset();
	modsMin.erase(id);
}

bool JITRunnableMin(long id) {
	auto it = modsMin.find(id);
	if (it == modsMin.end()) return 0;

	return !(modsMin[id].loop || modsMin[id].event);
}
}