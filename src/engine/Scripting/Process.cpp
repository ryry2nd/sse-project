#include <filesystem>
#include <string>
#include <fstream>
#include <vector>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_loadso.h>
#include <SDL3/SDL_error.h>

#include <spdlog/spdlog.h>

#define LIBRARY_PATH "lib"
#define CONFIG_NAME "config.txt"

extern "C" {
	long initModuleJIT(const char* path);
	void setupJIT(long id);
	long loopJIT(long id);
	long eventJIT(long id, SDL_Event *e);
	void shutdownJIT(long id);
	void removeJIT(long id);
	bool JITRunnable(long id);
}

static inline std::vector<std::string> split(const std::string& s, char delim)
{
	std::vector<std::string> out;
	std::stringstream ss(s);
	std::string item;

	while (std::getline(ss, item, delim))
		if (!item.empty())
			out.push_back(item);
	return out;
}

struct EnginePackage
{
	std::string name;
	std::string id;
	std::string version;
	std::string path;
	std::vector<std::string> deps;
	long llvmLocation;
};

static bool shouldShutDown = true;

std::vector<EnginePackage> pkgs;

extern "C" {
	bool ScriptingShouldStop() {
		if (shouldShutDown) {
			spdlog::info("No loop or event functions implemented. Shutting down");
			return true;
		}
		return false;
	}

	void ScriptingCreate(const char *path)
	{
		if (!std::filesystem::exists(path)) {
			spdlog::error("Module, {} does not exist", path);
			return;
		}

		std::filesystem::path confPath = std::filesystem::path(path) / CONFIG_NAME;

		if (!std::filesystem::exists(confPath)) {
			spdlog::error("Module, {} does not have a {}", path, CONFIG_NAME);
			return;
		}

		std::ifstream ifs(confPath);

		if (!ifs.is_open()) {
			spdlog::error("Module, {} could not open {}", path, CONFIG_NAME);
			return;
		}

		auto pkg = EnginePackage();

		std::string line;
		while (std::getline(ifs, line))
		{
			if (line.empty() || line[0] == '#')
				continue;

			auto eq = line.find('=');
			if (eq == std::string::npos)
				continue;

			std::string key = line.substr(0, eq);
			std::string value = line.substr(eq + 1);

			if (key == "ID")
				pkg.id = value;
			else if (key == "SHORT_NAME")
				pkg.name = value;
			else if (key == "VERSION")
				pkg.version = value;
			else if (key == "DEPS")
				pkg.deps = split(value, ';');
		}
		pkg.path = path;

		std::string codeFile;
		bool hasCode = false;
		for (const auto &entry : std::filesystem::directory_iterator(path)) {
			std::string ext = entry.path().extension().string();
			if (ext == ".bc" || ext == ".ll" || ext == ".o" || ext == ".obj" || ext == ".so" || ext == ".dll") {
				hasCode = true;
				codeFile = entry.path().string();
				break;
			}
		}


		if (!hasCode) return;

		long llvmLocation = initModuleJIT(codeFile.c_str());
		pkg.llvmLocation = llvmLocation;

		if (llvmLocation < 0) return;

		if (!JITRunnable(llvmLocation)) {
			shouldShutDown = false;
		}

		try {
			setupJIT(llvmLocation);
		}
		catch (const std::exception& e) {
			spdlog::error(e.what());
			#ifdef DEBUG
			throw e;
			#endif
		}

		spdlog::info("Successfully started {}", pkg.id);
		pkgs.push_back(std::move(pkg));
	}


	void ScriptingRunLoop() {
		for (auto &pkg : pkgs) {
			try {
				loopJIT(pkg.llvmLocation);
			}
			catch (const std::exception& e) {
				spdlog::error(e.what());
				#ifdef DEBUG
				throw e;
				#endif
			}
		}
	}

	void ScriptingRunEvent() {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			for (auto &pkg : pkgs) {
				try {
					eventJIT(pkg.llvmLocation, &e);
				}
				catch (const std::exception& e) {
					spdlog::error(e.what());
					#ifdef DEBUG
					throw e;
					#endif
				}
			}
		}
	}


	void ScriptingDestroy()
	{
		for (auto &pkg : pkgs) {
			try {
				shutdownJIT(pkg.llvmLocation);
			}
			catch (const std::exception& e) {
				spdlog::error(e.what());
				#ifdef DEBUG
				throw e;
				#endif
			}

			removeJIT(pkg.llvmLocation);

			spdlog::info("Successfully shut down module: {}", pkg.id);
		}

		pkgs.clear();
	}
}