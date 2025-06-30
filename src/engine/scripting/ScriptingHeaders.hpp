#pragma once

#include <sol/sol.hpp>
#include <filesystem>
#include <string>

#include "../objects/Objects.hpp"
#include "../customMath/Bigint.hpp"
#include "../customMath/BigVec.hpp"
#include "../HelperFunctions.hpp"

using namespace Objects;
using namespace Rendering;
using namespace OpenGl;

namespace LuaHeaders
{

    struct LuaRenderObject : public RenderObject
    {
        sol::table lua_instance;

        LuaRenderObject() = default;
        LuaRenderObject(Shader *shader, Image *image)
            : RenderObject(shader, image) {}

        void appendUpdate(const float &deltaTime) override
        {
            if (!lua_instance.valid())
            {
                RenderObject::appendUpdate(deltaTime);
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
                RenderObject::appendUpdate(deltaTime);
            }
        }

        using RenderObject::cullPriority;
        using RenderObject::meshes;
        using RenderObject::setupObject;
    };

    struct LuaMeshChunks : public MeshChunks
    {
        LuaMeshChunks(Shader *shader, Image *image)
            : MeshChunks(shader, image) {}
        using MeshChunks::cullPriority;
        using MeshChunks::meshes;
        using MeshChunks::setupObject;
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

        void includeScripts(std::string lua_dir)
        {
            for (const auto &entry : std::filesystem::directory_iterator(lua_dir))
            {
                if (entry.is_regular_file())
                {
                    auto path = entry.path();
                    if (path.extension() == ".lua")
                    {
                        std::cout << "Loading Lua script: " << path << std::endl;
                        try
                        {
                            lua.script_file(path.string());
                        }
                        catch (const sol::error &e)
                        {
                            std::cerr << "Error loading " << path << ": " << e.what() << std::endl;
                        }
                    }
                }
            }
        }

        sol::function getFunction(std::string func)
        {
            return lua[func];
        }

    private:
        sol::state lua;
        void includeGlm()
        {
            sol::table vecs = lua.create_table();
            // vecs.set_function("length", &glm::length);
            // vecs.set_function("normalize", &glm::normalize);
            vecs.new_usertype<glm::vec2>("vec2", sol::constructors<glm::vec2(), glm::vec2(float, float)>(), "x", &glm::vec2::x, "y", &glm::vec2::y, sol::meta_function::addition, [](const glm::vec2 &a, const glm::vec2 &b)
                                         { return a + b; }, sol::meta_function::subtraction, [](const glm::vec2 &a, const glm::vec2 &b)
                                         { return a - b; }, sol::meta_function::multiplication, [](const glm::vec2 &a, float b)
                                         { return a * b; }, // vector * scalar
                                         sol::meta_function::division, [](const glm::vec2 &a, float b)
                                         { return a / b; },

                                         sol::meta_function::equal_to, [](const glm::vec2 &a, const glm::vec2 &b)
                                         { return a == b; });
            vecs.new_usertype<glm::vec3>("vec3", sol::constructors<glm::vec3(), glm::vec3(float, float, float)>(), "x", &glm::vec3::x, "y", &glm::vec3::y, "z", &glm::vec3::z, sol::meta_function::addition, [](const glm::vec3 &a, const glm::vec3 &b)
                                         { return a + b; }, sol::meta_function::subtraction, [](const glm::vec3 &a, const glm::vec3 &b)
                                         { return a - b; }, sol::meta_function::multiplication, [](const glm::vec3 &a, float b)
                                         { return a * b; }, // vector * scalar
                                         sol::meta_function::division, [](const glm::vec3 &a, float b)
                                         { return a / b; },

                                         sol::meta_function::equal_to, [](const glm::vec3 &a, const glm::vec3 &b)
                                         { return a == b; });
            lua["glm"] = vecs;
        }
        void includeBigVars()
        {
            sol::table bigvars = lua.create_table();
            bigvars.new_usertype<Bigint>("Bigint",
                                         sol::constructors<Bigint(), Bigint(int), Bigint(float), Bigint(double), Bigint(long), Bigint(std::string)>(),
                                         "isZero", &Bigint::isZero, "getAbs", &Bigint::getAbs, "sqrt", &Bigint::sqrt, "toDouble", &Bigint::toDouble, "toFloat", &Bigint::toFloat, "toString", &Bigint::toString, "getHoweverManyDigits", &Bigint::getHoweverManyDigits,
                                         // Operator overloading
                                         sol::meta_function::addition, &Bigint::operator+,
                                         sol::meta_function::subtraction, &Bigint::operator-,
                                         sol::meta_function::multiplication, &Bigint::operator*,
                                         sol::meta_function::division, &Bigint::operator/,
                                         sol::meta_function::less_than, &Bigint::operator<,
                                         sol::meta_function::less_than_or_equal_to, &Bigint::operator<=,
                                         sol::meta_function::equal_to, &Bigint::operator==,
                                         sol::meta_function::to_string, &Bigint::toString);

            bigvars.new_usertype<BigVec3>("BigVec3", sol::constructors<BigVec3(), BigVec3(glm::vec3), BigVec3(Bigint), BigVec3(Bigint, Bigint, Bigint)>(), "getMaxAbs", &BigVec3::getMaxAbs, "getMinAbs", &BigVec3::getMinAbs, "isZero", &BigVec3::isZero, "x", &BigVec3::x, "y", &BigVec3::y, "z", &BigVec3::z,
                                          // Meta operator overloads
                                          sol::meta_function::addition, [](const BigVec3 &a, const BigVec3 &b)
                                          { return a + b; }, sol::meta_function::subtraction, [](const BigVec3 &a, const BigVec3 &b)
                                          { return a - b; }, sol::meta_function::multiplication, [](const BigVec3 &a, const BigVec3 &b)
                                          { return a * b; }, sol::meta_function::division, [](const BigVec3 &a, const BigVec3 &b)
                                          { return a / b; });

            lua["bigvars"] = bigvars;
        }

        void includeObjects()
        {
            sol::table objects = lua.create_table();
            objects.new_usertype<Shader>("Shader");
            objects.new_usertype<Image>("Image");
            objects.new_usertype<Camera>("Camera", "position", &Camera::position, "rotation", &Camera::rotation, "fov", &Camera::fov);
            objects.new_usertype<Mesh>("Mesh", "posOffset", &Mesh::posOffset, "rotOffset", &Mesh::rotOffset, "sizeOffset", &Mesh::sizeOffset);
            objects["CullPriority"] = lua.create_table_with(
                "Low", CullPriority::Low,
                "Medium", CullPriority::Medium,
                "High", CullPriority::High,
                "Max", CullPriority::Max);
            objects.new_usertype<LuaRenderObject>("RenderObject",
                                                  sol::constructors<LuaRenderObject(), LuaRenderObject(Shader *, Image *)>(),
                                                  sol::base_classes, sol::bases<RenderObject>(),
                                                  "lua_instance", &LuaRenderObject::lua_instance,
                                                  "position", &LuaRenderObject::position,
                                                  "rotation", &LuaRenderObject::rotation,
                                                  "meshes", &LuaRenderObject::meshes,
                                                  "cullPriority", &LuaRenderObject::cullPriority,
                                                  "appendUpdate", &LuaRenderObject::appendUpdate,
                                                  "setupObject", &LuaRenderObject::setupObject);
            objects.new_usertype<LuaMeshChunks>("MeshChunks",
                                                sol::constructors<LuaMeshChunks(Shader *, Image *)>(),
                                                sol::base_classes, sol::bases<MeshChunks, RenderObject>(),
                                                "position", &LuaMeshChunks::position,
                                                "rotation", &LuaMeshChunks::rotation,
                                                "meshes", &LuaMeshChunks::meshes,
                                                "cullPriority", &LuaMeshChunks::cullPriority,
                                                "setupObject", &LuaMeshChunks::setupObject);
            lua["Objects"] = objects;
        }
    };
}