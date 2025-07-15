#include "ScriptingHeaders.hpp"
#include <yaml-cpp/yaml.h>
#include <filesystem>

using namespace ScriptingHeaders;

GameLibrary::GameLibrary(const std::string &path)
{
    YAML::Node config = YAML::LoadFile(path + "/config.yaml");

    name = config["name"].as<std::string>();
    id = config["id"].as<std::string>();
    version = config["version"].as<std::string>();

    if (config["permissions"])
    {
        if (config["permissions"]["include_glm"] && config["permissions"]["include_glm"].as<bool>())
        {
            includeGlm();
        }
        if (config["permissions"]["include_bigvars"] && config["permissions"]["include_bigvars"].as<bool>())
        {
            includeBigVars();
        }
        if (config["permissions"]["include_rendering"] && config["permissions"]["include_rendering"].as<bool>())
        {
            includeRendering();
        }
        if (config["permissions"]["include_objects"] && config["permissions"]["include_objects"].as<bool>())
        {
            includeObjects();
        }
    }
    if (config["assets"])
    {
        if (config["assets"]["fonts"])
        {
            size_t size = config["assets"]["fonts"].size();
            fonts.reserve(size);
            for (size_t i = 0; i < size; i++)
            {
                fonts[config["assets"]["fonts"][i]["name"].as<std::string>()] = new Rendering::Font(path + "/" + config["assets"]["fonts"][i]["path"].as<std::string>(), config["assets"]["fonts"][i]["size"].as<int>());
            }
        }
        if (config["assets"]["textures"])
        {
            size_t size = config["assets"]["textures"].size();
            images.reserve(size);

            for (size_t i = 0; i < size; i++)
            {
                images[config["assets"]["textures"][i]["name"].as<std::string>()] = Rendering::defaultImageAPI->makeNewImage(path + "/" + config["assets"]["textures"][i]["path"].as<std::string>());
            }
        }
    }

    if (config["opengl_shaders"])
    {
        size_t size = config["opengl_shaders"].size();
        shaders.reserve(size);

        for (size_t i = 0; i < size; i++)
        {
            shaders[config["opengl_shaders"][i]["name"].as<std::string>()] = Rendering::defaultShaderAPI->makeNewShader((path + "/" + config["opengl_shaders"][i]["vertPath"].as<std::string>()).c_str(), (path + "/" + config["opengl_shaders"][i]["fragPath"].as<std::string>()).c_str());
        }
    }

    lua.open_libraries(sol::lib::base, sol::lib::table, sol::lib::utf8, sol::lib::string, sol::lib::math);

    lua.set_function("get_package", [&](const std::string &package_name, const std::string &func_name) -> sol::function
                        {
        if (packages.find(package_name) == packages.end()) {
            throw std::runtime_error("Package not found: " + package_name);
        }
        sol::function func = packages[package_name]->lua[func_name];
        if (!func.valid()) {
            throw std::runtime_error("Function not found in package: " + func_name);
        }
        return func; });

    lua.set_function("get_shader", [&](const std::string &shaderName)
                        { return shaders[shaderName]; });

    lua.set_function("get_image", [&](const std::string &imageName)
                        { return images[imageName]; });

    lua.set_function("get_fonts", [&](const std::string &fontName)
                        { return shaders[fontName]; });

    if (config["lua_scripts"])
    {
        for (size_t i = 0; i < config["lua_scripts"].size(); i++)
        {
            lua.script_file(path + "/" + config["lua_scripts"][i]["path"].as<std::string>());
        }
    }
}

GameLibrary::~GameLibrary()
{
    packages.erase(id);
    for (auto &[name, ptr] : fonts)
        delete ptr;
    for (auto &[name, ptr] : shaders)
        delete ptr;
    for (auto &[name, ptr] : images)
        delete ptr;
}