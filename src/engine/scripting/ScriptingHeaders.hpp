#pragma once
#include <string>
#include <vector>

struct SDL_SharedObject;

namespace ScriptingHeaders
{
    typedef void(*FuncType)();
    class Package
    {
    public:
        std::string name;
        std::string id;
        std::string version;
        std::string path;

        static std::vector<Package *> packages;
        static void LoopFunctions();

        Package(const std::string &path);
        ~Package();

    private:
        SDL_SharedObject* lib;
        FuncType loopFunc;

        void runLoopFunction();
    };
}