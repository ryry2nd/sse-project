#include "ScriptingHeaders.hpp"
#include <yaml-cpp/yaml.h>
#include <filesystem>
#include <string>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_loadso.h>
#include <SDL3/SDL_error.h>

using namespace ScriptingHeaders;


#define LIBRARY_PATH "lib"
#define CONFIG_NAME "/config.yaml"

extern "C" {
	long initModuleJIT(const char* path);
	void setupJIT(long id);
	long loopJIT(long id);
	long eventJIT(long id, SDL_Event *e, bool *running);
	void shutdownJIT(long id);
	void removeJIT(long id);
}

std::vector<EnginePackage *> EnginePackage::packages;

static bool shouldShutDown = true;
bool EnginePackage::ShouldStop() {
	if (shouldShutDown) {
		spdlog::info("No loop or event functions implemented. Shutting down");
		return true;
	}
	shouldShutDown = true;
	return false;
}


void EnginePackage::LoopFunctions() {
	for (EnginePackage * pkg : packages) {
		pkg->runLoopFunction();
	}
}

void EnginePackage::EventFunctions(bool *running) {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		for (EnginePackage * pkg : packages) {
			pkg->runEventFunction(&event, running);
		}
	}
}

EnginePackage::EnginePackage(const std::string &path)
{
	if (!std::filesystem::exists(path)) {
		spdlog::error("Path does not exist");
		return;
	}

	packages.push_back(this);
	if (!std::filesystem::exists(path + CONFIG_NAME)) {
		spdlog::error("Path does not have a: {}", CONFIG_NAME);
		return;
	}

	YAML::Node config = YAML::LoadFile(path + CONFIG_NAME);

	name = config["name"].as<std::string>();
	id = config["id"].as<std::string>();
	version = config["version"].as<std::string>();
	this->path = path;
	std::string module_filename = std::filesystem::path(path).filename().string();

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

	try {
		setupJIT(llvmLocation);
	}
	catch (const std::exception& e) {
		#ifdef DEBUG
		throw e;
		#else
		spdlog::error(e.what());
		#endif
	}

	spdlog::info("Successfully started {}", id);
}


void EnginePackage::runLoopFunction() {
	try {
		if (!loopJIT(llvmLocation)) shouldShutDown = false;
	}
	catch (const std::exception& e) {
		#ifdef DEBUG
		throw e;
		#else
		spdlog::error(e.what());
		#endif
	}
}

void EnginePackage::runEventFunction(SDL_Event *e, bool *running) {
	try {
		if(!eventJIT(llvmLocation, e, running)) shouldShutDown = false;
	}
	catch (const std::exception& e) {
		#ifdef DEBUG
		throw e;
		#else
		spdlog::error(e.what());
		#endif
	}
}


EnginePackage::~EnginePackage()
{
	auto it = std::find(packages.begin(), packages.end(), this);
	if (it != packages.end())
		packages.erase(it);

	try {
		shutdownJIT(llvmLocation);
	}
	catch (const std::exception& e) {
		spdlog::error(e.what());
	}

	removeJIT(llvmLocation);

	spdlog::info("Successfully shut down module: {}", id);
}