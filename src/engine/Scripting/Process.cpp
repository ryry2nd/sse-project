#include <filesystem>
#include <queue>
#include <string>
#include <fstream>
#include <vector>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_loadso.h>
#include <SDL3/SDL_error.h>

#include <spdlog/spdlog.h>

#define LIBRARY_PATH "lib"
#define CONFIG_NAME "config.txt"
#define MODULES_PATH "modules"

namespace fs = std::filesystem;

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

std::vector<EnginePackage> pkgs;

void topoSort() {
    std::unordered_map<std::string, int> index;
    std::unordered_map<std::string, int> indegree;
    std::unordered_map<std::string, std::vector<std::string>> graph;

    // map id -> index in vector
    for (int i = 0; i < (int)pkgs.size(); ++i) {
        index[pkgs[i].id] = i;
        indegree[pkgs[i].id] = 0;
    }

    // build graph
    for (const auto& m : pkgs) {
        for (const auto& dep : m.deps) {
            if (!index.contains(dep)) {
                spdlog::error("missing dependency {} for module {}", m.id, dep);
                std::exit(1);
            }

            graph[dep].push_back(m.id);
            indegree[m.id]++;
        }
    }

    std::queue<std::string> q;

    for (const auto& [id, deg] : indegree) {
        if (deg == 0) q.push(id);
    }

    std::vector<EnginePackage> ordered;
    ordered.reserve(pkgs.size());

    while (!q.empty()) {
        std::string cur = q.front();
        q.pop();

        ordered.push_back(pkgs[index[cur]]);

        for (const auto& nxt : graph[cur]) {
            if (--indegree[nxt] == 0)
                q.push(nxt);
        }
    }

    if (ordered.size() != pkgs.size()) {
        spdlog::error("cyclic dependency detected");
        std::exit(1);
    }

    // overwrite input vector in-place
    pkgs = std::move(ordered);
}

static bool shouldShutDown = true;

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
		for (const auto& entry : fs::directory_iterator(MODULES_PATH)) {
			if (entry.is_directory()) {
				std::string mod_path = entry.path().string();
				std::filesystem::path confPath = std::filesystem::path(mod_path) / CONFIG_NAME;

				if (!std::filesystem::exists(confPath)) {
					continue;
				}

				std::ifstream ifs(confPath);

				if (!ifs.is_open()) {
					spdlog::error("Module, {} could not open {}", mod_path, CONFIG_NAME);
					continue;
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
				pkg.path = mod_path;

				std::string codeFile;
				bool hasCode = false;
				for (const auto &entry : std::filesystem::directory_iterator(mod_path)) {
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

				pkgs.push_back(std::move(pkg));
			}
		}

		topoSort(); // ToDo important, change it so this instead takes in the path and only loads the dependents and dependencies

		for (auto &pkg : pkgs) {
			try {
				setupJIT(pkg.llvmLocation);
			}
			catch (const std::exception& e) {
				spdlog::error(e.what());
				#ifdef DEBUG
				throw e;
				#endif
			}

			spdlog::info("Successfully started {}", pkg.id);
		}
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