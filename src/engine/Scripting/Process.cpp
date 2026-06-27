#include "ScriptingHeaders.hpp"
#include <filesystem>
#include <string>
#include <fstream>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_loadso.h>
#include <SDL3/SDL_error.h>

#include <spdlog/spdlog.h>

using namespace ScriptingHeaders;


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

static bool shouldShutDown = true;

bool EnginePackage::ShouldStop() {
	if (shouldShutDown) {
		spdlog::info("No loop or event functions implemented. Shutting down");
		return true;
	}
	return false;
}

static inline std::vector<std::string> split(const std::string& s, char delim)
{
    std::vector<std::string> out;
    std::stringstream ss(s);
    std::string item;

    while (std::getline(ss, item, delim))
    {
        if (!item.empty())
            out.push_back(item);
    }
    return out;
}


EnginePackage::EnginePackage(const std::string &path)
{
	if (!std::filesystem::exists(path)) {
		spdlog::error("Module, {} does not exist", path);
		return;
	}
	if (!std::filesystem::exists(path + "/" + CONFIG_NAME)) {
		spdlog::error("Module, {} does not have a {}", path, CONFIG_NAME);
		return;
	}

	std::ifstream ifs(path + "/" + CONFIG_NAME);

	if (!ifs.is_open()) {
        spdlog::error("Module, {} could not open {}", path, CONFIG_NAME);
		return;
	}


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
            id = value;
        else if (key == "SHORT_NAME")
            name = value;
        else if (key == "VERSION")
            version = value;
        else if (key == "DEPS")
            deps = split(value, ';');
    }
	this->path = path;

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

	llvmLocation = initModuleJIT(codeFile.c_str());

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

	spdlog::info("Successfully started {}", id);
}


void EnginePackage::LoopFunction() {
	try {
		loopJIT(llvmLocation);
	}
	catch (const std::exception& e) {
		spdlog::error(e.what());
		#ifdef DEBUG
		throw e;
		#endif
	}
}

void EnginePackage::EventFunction(SDL_Event *e) {
	try {
		eventJIT(llvmLocation, e);
	}
	catch (const std::exception& e) {
		spdlog::error(e.what());
		#ifdef DEBUG
		throw e;
		#endif
	}
}


EnginePackage::~EnginePackage()
{
	try {
		shutdownJIT(llvmLocation);
	}
	catch (const std::exception& e) {
		spdlog::error(e.what());
		#ifdef DEBUG
		throw e;
		#endif
	}

	removeJIT(llvmLocation);

	spdlog::info("Successfully shut down module: {}", id);
}