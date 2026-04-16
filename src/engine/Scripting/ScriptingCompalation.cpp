#include "ScriptingHeaders.hpp"
#include <yaml-cpp/yaml.h>
#include <filesystem>
#include <iostream>
#include <string>
#include <memory>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_loadso.h>
#include <SDL3/SDL_error.h>

using namespace ScriptingHeaders;

#define COMPILER_PATH "clang/bin/clang++"
#define COMPILED_OUT_PATH "cache/compiled_modules"
#define LIBRARY_PATH "libs"
#define CPP_VERSION "20"

#define INCLUDE_PATH "include"

#ifdef _WIN32
#define POPEN _popen
#define PCLOSE _pclose
#define EXE_EXT ".exe"
#define BIN_PREFIX ""
#else
#define POPEN popen
#define PCLOSE pclose
#define EXE_EXT ""
#define BIN_PREFIX "./"
#endif

#define LIBRARY_SUFFIX ".bin"

std::vector<Package *> Package::packages;

// scarry code, watch out, can execute random commands. DO NOT GIVE THIS TO THE HEADER
int compileCode(const std::string& arguments) {
    std::string command = std::string(BIN_PREFIX) + COMPILER_PATH + EXE_EXT + " " + arguments;
    std::unique_ptr<FILE, int(*)(FILE*)> pipe(
        POPEN(command.c_str(), "r"), 
        static_cast<int(*)(FILE*)>(PCLOSE)
    );
    if (!pipe) {
        std::cerr << "Failed to run command";
        return 1;
    }
    return 0;
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
    if (!std::filesystem::exists(path)) throw std::runtime_error("Path does not exist");

    std::filesystem::create_directories(std::string(BIN_PREFIX) + std::string(COMPILED_OUT_PATH));

    packages.push_back(this);
    YAML::Node config = YAML::LoadFile(path + "/config.yaml");

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
        
        if (compileCode(command)) {return;}

    }

    lib = SDL_LoadObject((std::string(COMPILED_OUT_PATH) + "/" + module_filename + std::string(LIBRARY_SUFFIX)).c_str());
    if (!lib) {
        std::cerr << "Failed to load library: " << SDL_GetError() << "\n";
        return;
    }

    FuncType func = (FuncType)SDL_LoadFunction(lib, "setup");
    try {
        if (func) func();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }   

    loopFunc = (FuncType)SDL_LoadFunction(lib, "loop");
    eventFunc = (EventType)SDL_LoadFunction(lib, "event");
}

void Package::runLoopFunction() {
    try {
        if (loopFunc) {loopFunc();}
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
}

void Package::runEventFunction(SDL_Event *event, bool *running) {
    try {
        if (eventFunc) {eventFunc(event, running);}
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
}

Package::~Package()
{
    auto it = std::find(packages.begin(), packages.end(), this);
    if (it != packages.end())
        packages.erase(it);

    if (lib) {
        FuncType shutDownFunc = (FuncType)SDL_LoadFunction(lib, "shutdown");
        
        try {
            if (shutDownFunc) shutDownFunc();
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
        }

        SDL_UnloadObject(lib);
    }
}