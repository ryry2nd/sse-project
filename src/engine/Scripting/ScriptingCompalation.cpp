#include "ScriptingHeaders.hpp"
#include "spdlog/spdlog.h"
#include <yaml-cpp/yaml.h>
#include <filesystem>
#include <string>
#include <memory>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_loadso.h>
#include <SDL3/SDL_error.h>

using namespace ScriptingHeaders;


#define LIBRARY_PATH "libs"
#define CPP_VERSION "20"

#define INCLUDE_PATH "include"

#if USECMDCLANG
#define COMPILER_PATH "clang++"
#else
#define COMPILER_PATH "clang/bin/clang++"
#endif

#define COMPILED_OUT_PATH "cache/compiled_modules"


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
#define LIBRARY_SUFFIX ".so"
#if USECMDCLANG
#define BIN_PREFIX ""
#else
#define BIN_PREFIX "./"
#endif
#endif

#define START_NAME "setup"
#define LOOP_NAME "loop"
#define STOP_NAME "shutdown"
#define EVENT_NAME "event"

#define CONFIG_NAME "/config.yaml"


std::vector<Package *> Package::packages;

// scarry code, watch out, can execute random commands. DO NOT GIVE THIS TO THE HEADER
std::string compileCode(const std::string& arguments) {
    std::string command = std::string(BIN_PREFIX) + COMPILER_PATH + EXE_EXT + " " + arguments + " 2>&1";

    std::unique_ptr<FILE, int(*)(FILE*)> pipe(
        POPEN(command.c_str(), "r"),
        static_cast<int(*)(FILE*)>(PCLOSE)
    );

    if (!pipe) {
        return "Failed to run command";
    }

    std::string output;
    char buffer[256];

    while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr) {
        output += buffer;
    }
    return output;
}

void Package::LoopFunctions() {
    for (Package * pkg : packages) {
        pkg->runLoopFunction();
    }
}

void Package::EventFunctions(bool *running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        for (Package * pkg : packages) {
            pkg->runEventFunction(&event, running);
        }
    }
}

Package::Package(const std::string &path)
{
    if (!std::filesystem::exists(path)) {
        spdlog::error("Path does not exist");
        return;
    }

    std::filesystem::create_directories(std::string(BIN_PREFIX) + std::string(COMPILED_OUT_PATH));

    packages.push_back(this);
    YAML::Node config = YAML::LoadFile(path + CONFIG_NAME);

    name = config["name"].as<std::string>();
    id = config["id"].as<std::string>();
    version = config["version"].as<std::string>();
    this->path = path;
    std::string module_filename = std::filesystem::path(path).filename().string();

    bool hasCppFiles = false;
    if (std::filesystem::exists(path + "/src")) {
        for (const auto &entry : std::filesystem::directory_iterator(path + "/src")) {
            if (entry.path().extension() == ".cpp") {
                hasCppFiles = true;
                break;
            }
        }
    }

    if (!hasCppFiles) return;

    if (!std::filesystem::exists(std::string(COMPILED_OUT_PATH) + "/" + module_filename + std::string(LIBRARY_SUFFIX))) {
        std::string libInclude = "";

        if (config["include"])
        {
            if (config["include"]["Rendering"] && config["include"]["Rendering"].as<bool>())
            {
                libInclude += " -lRenderingBase";
            }
            if (config["include"]["BigObjects"] && config["include"]["BigObjects"].as<bool>())
            {
                libInclude += " -lBigObjects";
            }
            if (config["include"]["SDL3"] && config["include"]["SDL3"].as<bool>()) {
                libInclude += " -lSDL3";
            }
        }

        std::string command = "-w -fPIC -shared -std=c++" + std::string(CPP_VERSION) + \
            " -o " + std::string(COMPILED_OUT_PATH) + "/" + module_filename + std::string(LIBRARY_SUFFIX) +
            " " + path + "/src/*.cpp -L" +
            std::string(LIBRARY_PATH) + libInclude + " -I" + std::string(INCLUDE_PATH) +
            " -DMODULE_PATH=\\\"" + path + "\\\"";
        
        std::string out = compileCode(command);
        if (out.size()) {
            spdlog::error("Program: {} failed to compile and gave the error:\n{}", id, out);
            return;
        }
        spdlog::info("Successfully compiled {}", id);

    }

    lib = SDL_LoadObject((std::string(COMPILED_OUT_PATH) + "/" + module_filename + std::string(LIBRARY_SUFFIX)).c_str());
    if (!lib) {
        spdlog::error("Failed to load library: {}", SDL_GetError());
        return;
    }

    FuncType func = (FuncType)SDL_LoadFunction(lib, START_NAME);
    try {
        if (func) func();
    }
    catch (const std::exception& e) {
        spdlog::error(e.what());
    }   

    loopFunc = (FuncType)SDL_LoadFunction(lib, LOOP_NAME);
    eventFunc = (EventType)SDL_LoadFunction(lib, EVENT_NAME);

    spdlog::info("Successfully started {}", id);
}

void Package::runLoopFunction() {
    try {
        if (loopFunc) {loopFunc();}
    }
    catch (const std::exception& e) {
        spdlog::error(e.what());
    }
}

void Package::runEventFunction(SDL_Event *event, bool *running) {
    try {
        if (eventFunc) {eventFunc(event, running);}
    }
    catch (const std::exception& e) {
        spdlog::error(e.what());
    }
}

Package::~Package()
{
    auto it = std::find(packages.begin(), packages.end(), this);
    if (it != packages.end())
        packages.erase(it);

    if (lib) {
        FuncType shutDownFunc = (FuncType)SDL_LoadFunction(lib, STOP_NAME);
        
        try {
            if (shutDownFunc) shutDownFunc();
        }
        catch (const std::exception& e) {
            spdlog::error(e.what());
        }

        SDL_UnloadObject(lib);
    }
}