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

std::unordered_map<long, MyModule> mods;


extern "C" {
long initModuleJIT(const char* path) {
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

	mods[currID] = std::move(mod);
	return currID++;
}
void setupJIT(long id) {
	auto it = mods.find(id);
	if (it == mods.end()) return;

	if (!mods[id].setup) return;

	mods[id].setup();
}
bool loopJIT(long id) {
	auto it = mods.find(id);
	if (it == mods.end()) return 1;

	if (!mods[id].loop)	return 1;

	mods[id].loop();
	return 0;
}
bool eventJIT(long id, SDL_Event *e, bool *running) {
	auto it = mods.find(id);
	if (it == mods.end()) return 1;

	if (!mods[id].event) {
		return 1;
	}
	mods[id].event(e, running);
	return 0;
}
void shutdownJIT(long id) {
	auto it = mods.find(id);
	if (it == mods.end()) return;

	if (!mods[id].shutdown) {
		return;
	}
	mods[id].shutdown();
}
void removeJIT(long id) {
	auto it = mods.find(id);
	if (it == mods.end()) return;

	mods[id].setup = nullptr;
	mods[id].loop = nullptr;
	mods[id].event = nullptr;
	mods[id].shutdown = nullptr;
	mods[id].runtime.reset();
	mods.erase(id);
}
}