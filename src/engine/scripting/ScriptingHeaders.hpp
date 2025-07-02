#pragma once

#include <sol/sol.hpp>
#include <filesystem>
#include <string>
#include <yaml-cpp/yaml.h>

#include "../objects/Objects.hpp"
#include "../customMath/Bigint.hpp"
#include "../customMath/BigVec.hpp"
#include "../HelperFunctions.hpp"

namespace LuaHeaders
{
    struct LuaRenderObject : public Objects::RenderObject
    {
        sol::table lua_instance;

        LuaRenderObject() = default;
        LuaRenderObject(Rendering::Shader *shader, Rendering::Image *image)
            : Objects::RenderObject(shader, image) {}

        void appendUpdate(const float &deltaTime) override
        {
            if (!lua_instance.valid())
            {
                Objects::RenderObject::appendUpdate(deltaTime);
                return;
            }

            sol::function override = lua_instance["appendUpdate"];
            if (override.valid())
            {
                sol::protected_function_result result = override(lua_instance, deltaTime);
                if (!result.valid())
                {
                    sol::error err = result;
                    std::cerr << "[C++] Lua error in appendUpdate: " << err.what() << "\n";
                }
            }
            else
            {
                Objects::RenderObject::appendUpdate(deltaTime);
            }
        }

        using Objects::RenderObject::cullPriority;
        using Objects::RenderObject::meshes;
        using Objects::RenderObject::setupObject;
    };

    struct LuaMeshChunks : public Objects::MeshChunks
    {
        LuaMeshChunks(Rendering::Shader *shader, Rendering::Image *image)
            : Objects::MeshChunks(shader, image) {}
        using Objects::MeshChunks::cullPriority;
        using Objects::MeshChunks::meshes;
        using Objects::MeshChunks::setupObject;
    };

    enum class LuaLibEnum
    {
        glm,
        bigvars,
        objects
    };

    class LuaScriptLib
    {
    public:
        LuaScriptLib()
        {
            lua.open_libraries(sol::lib::base);
            lua.open_libraries(sol::lib::table);
        }
        void include(LuaLibEnum libName)
        {
            switch (libName)
            {
            default:
                throw std::runtime_error("Library not found");
            case LuaLibEnum::glm:
                includeGlm();
                break;
            case LuaLibEnum::bigvars:
                includeBigVars();
                break;
            case LuaLibEnum::objects:
                includeObjects();
                break;
            };
        }
        template <typename Func>
        void add_custom_function(const std::string &name, Func &&func)
        {
            lua.set_function(name, std::forward<Func>(func));
        }
        template <typename Func>
        void includeInitialized(const std::string &name, Func &&func)
        {
            lua[name] = &func;
        }
        void includeScripts(std::string lua_dir);
        sol::function getFunction(std::string func)
        {
            return lua[func];
        }

    private:
        sol::state lua;
        void includeGlm();
        void includeBigVars();
        void includeObjects();
    };
}

struct GameLibrary
{
    std::string name;
    std::string id;
    std::string version;

    std::unordered_map<std::string, Rendering::Font *> fonts;
    std::unordered_map<std::string, Rendering::Shader *> shaders;
    std::unordered_map<std::string, Rendering::Image *> images;

    GameLibrary(const std::string &path)
    {
        YAML::Node config = YAML::LoadFile(path + "/config.yaml");
        name = config["name"].as<std::string>();
        id = config["id"].as<std::string>();
        version = config["version"].as<std::string>();

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

        if (config["shaders"])
        {
            size_t size = config["shaders"].size();
            shaders.reserve(size);

            for (size_t i = 0; i < size; i++)
            {
                shaders[config["shaders"][i]["name"].as<std::string>()] = Rendering::defaultShaderAPI->makeNewShader((path + "/" + config["shaders"][i]["vertPath"].as<std::string>()).c_str(), (path + "/" + config["shaders"][i]["fragPath"].as<std::string>()).c_str());
            }
        }
    }

    ~GameLibrary()
    {
        for (auto &[key, ptr] : fonts)
        {
            delete ptr;
        }
        fonts.clear();
        for (auto &[key, ptr] : shaders)
        {
            delete ptr;
        }
        shaders.clear();
        for (auto &[key, ptr] : images)
        {
            delete ptr;
        }
        images.clear();
    }
};