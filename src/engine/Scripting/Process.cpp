#include "ScriptingHeaders.hpp"
#include <yaml-cpp/yaml.h>
#include <filesystem>
#include <string>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_loadso.h>
#include <SDL3/SDL_error.h>

#include <spdlog/spdlog.h>

using namespace ScriptingHeaders;


#define LIBRARY_PATH "lib"
#define CONFIG_NAME "/config.yaml"

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

EnginePackage::EnginePackage(const std::string &path)
{
	if (!std::filesystem::exists(path)) {
		spdlog::error("Path does not exist");
		return;
	}
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