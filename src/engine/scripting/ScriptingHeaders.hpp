#pragma once

#include <sol/sol.hpp>
#include <string>

#include "../HelperFunctions.hpp"

namespace ScriptingHeaders
{
    struct GameLibrary
    {
        std::string name;
        std::string id;
        std::string version;

        sol::state lua;

        std::unordered_map<std::string, Rendering::Shader *> shaders;
        std::unordered_map<std::string, Rendering::Image *> images;
        std::unordered_map<std::string, Rendering::Font *> fonts;

        static std::unordered_map<std::string, GameLibrary *> packages;

        GameLibrary(const std::string &path);
        ~GameLibrary();

    private:
        void includeGlm();
        void includeRendering();
        void includeBigVars();
        void includeObjects();
    };
}