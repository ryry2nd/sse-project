#include "ScriptingHeaders.hpp"
#include <yaml-cpp/yaml.h>
#include <SDL3/SDL.h>
#include <filesystem>
#include <iostream>
#include <string>
#include <array>
#include <memory>

using namespace ScriptingHeaders;

#define COMPILER_PATH "clang/bin/clang++"
#define COMPILED_OUT_PATH "cache/compiled_modules"
#define LIBRARY_PATH "libs"
#define CPP_VERSION "20"

#define CUSTOM_MATH_PATH "include/customMath"
#define RENDERING_PATH "include/rendering"
#define OBJECT_PATH "include/objects"

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

std::vector<Package *> Package::packages;

// scarry code, watch out, can execute random commands. DO NOT GIVE THIS TO THE HEADER
void compileCode(const std::string& arguments) {
    std::string command = std::string(BIN_PREFIX) + COMPILER_PATH + EXE_EXT + " " + arguments;
    std::unique_ptr<FILE, decltype(&PCLOSE)> pipe(POPEN(command.c_str(), "r"), PCLOSE);
    if (!pipe) {
        throw std::runtime_error("Failed to run command");
    }
}

void Package::LoopFunctions() {
    for (Package * pkg : packages) {
        pkg->runLoopFunction();
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

    if (!std::filesystem::exists(path+"/src")) return;

    if (!std::filesystem::exists(std::string(COMPILED_OUT_PATH) + "/" + module_filename + std::string(LIBRARY_SUFFIX))) {
        std::string headerPaths = "";
        std::string libInclude = "";

        if (config["include"])
        {
            if (config["include"]["customMath"] && config["include"]["customMath"].as<bool>())
            {
                headerPaths += " -I" + std::string(CUSTOM_MATH_PATH);
            }
            if (config["include"]["rendering"] && config["include"]["rendering"].as<bool>())
            {
                headerPaths += " -I" + std::string(RENDERING_PATH);
                libInclude += " -lrenderingBase";
            }
            if (config["include"]["objects"] && config["include"]["objects"].as<bool>())
            {
                headerPaths += " -I" + std::string(OBJECT_PATH);
                libInclude += " -lobjects";
            }
        }

        compileCode("-shared -std=c++" + std::string(CPP_VERSION) + \
            " -o " + std::string(COMPILED_OUT_PATH) + "/" + module_filename + std::string(LIBRARY_SUFFIX) + \
            + " " + path + "/src/*.cpp -L" + \
            std::string(LIBRARY_PATH) + headerPaths + libInclude + \
            " -DMODULE_PATH=\\\"" + path + "\\\"");

    }
    
    lib = SDL_LoadObject((std::string(COMPILED_OUT_PATH) + "/" + module_filename + std::string(LIBRARY_SUFFIX)).c_str());
    if (!lib) {
        std::cerr << "Failed to load library: " << SDL_GetError() << "\n";
        return;
    }

    FuncType func = (FuncType)SDL_LoadFunction(lib, "setup");
    if (!func) {
        std::cerr << "Failed to load function: " << SDL_GetError() << "\n";
        SDL_UnloadObject(lib);
        return;
    }

    loopFunc = (FuncType)SDL_LoadFunction(lib, "loop");

    func();
}

void Package::runLoopFunction() {
    if (loopFunc) {
        loopFunc();
    }
}

Package::~Package()
{
    auto it = std::find(packages.begin(), packages.end(), this);
    if (it != packages.end())
        packages.erase(it);
    SDL_UnloadObject(lib);
}