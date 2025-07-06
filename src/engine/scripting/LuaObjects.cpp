#include "ScriptingHeaders.hpp"

using namespace ScriptingHeaders;

std::unordered_map<std::string, GameLibrary *> GameLibrary::packages;

void GameLibrary::includeGlm()
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

void GameLibrary::includeBigVars()
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

void GameLibrary::includeRendering()
{
    using namespace Rendering;
    sol::table rendering = lua.create_table();

    rendering.new_usertype<Shader>("Shader", sol::no_constructor); //, "createUniform", &Shader::createUniform, "includeShader", &Shader::includeShader, "setUniform", &Shader::setUniform, "enableCulling", &Shader::enableCulling, "disableCulling", &Shader::disableCulling);
    rendering.new_usertype<Image>("Image", sol::no_constructor, "imageSizes", &Image::imageSizes);
    rendering.new_usertype<Mesh>("Mesh", sol::no_constructor, "posOffset", &Mesh::posOffset, "rotOffset", &Mesh::rotOffset, "sizeOffset", &Mesh::sizeOffset);
    lua["Rendering"] = rendering;
}

void GameLibrary::includeObjects()
{

    using namespace Objects;
    sol::table objects = lua.create_table();

    objects["CullPriority"] = lua.create_table_with(
        "Low", CullPriority::Low,
        "Medium", CullPriority::Medium,
        "High", CullPriority::High,
        "Max", CullPriority::Max);

    objects.new_usertype<BaseObject>("BaseObject", sol::constructors<BaseObject(), BaseObject(const BigVec3 &, const glm::vec3 &)>(),
                                     "position", &BaseObject::position, "rotation", &BaseObject::rotation,
                                     "children", &BaseObject::children, "getTruePos", &BaseObject::getTruePos,
                                     "makeParent", &BaseObject::makeParent, "parent", &BaseObject::parent, "removeParent", &BaseObject::removeParent);
    objects.new_usertype<Camera>("Camera", sol::constructors<Camera(), Camera(BigVec3 &, glm::vec3 &)>(),
                                 sol::base_classes, sol::bases<BaseObject>(),
                                 "fov", &Camera::fov, "near", &Camera::near, "far", &Camera::far,
                                 "convertToLocal", &Camera::convertToLocal, "getViewMatrix", &Camera::getViewMatrix,
                                 "getProjectionMatrix", &Camera::getProjectionMatrix, "getViewMatrix2d", &Camera::getProjectionMatrix2d,
                                 "getDownVector", &Camera::getDownVector, "getForwardVector", &Camera::getForwardVector, "getRightVector", &Camera::getRightVector);
    objects.new_usertype<LuaRenderObject>("RenderObject",
                                          sol::constructors<LuaRenderObject(), LuaRenderObject(Rendering::Shader *, Rendering::Shader *, Rendering::Image *)>(),
                                          sol::base_classes, sol::bases<BaseObject>(),
                                          "lua_instance", &LuaRenderObject::lua_instance,
                                          "meshes", &LuaRenderObject::meshes,
                                          "cullPriority", &LuaRenderObject::cullPriority,
                                          "appendUpdate", &LuaRenderObject::appendUpdate,
                                          "setupObject", &LuaRenderObject::setupObject);
    // objects.new_usertype<LuaMeshChunks>("MeshChunks",
    //                                     sol::constructors<LuaMeshChunks(Rendering::Shader *, Rendering::Image *)>(),
    //                                     sol::base_classes, sol::bases<MeshChunks, RenderObject>(),
    //                                     "position", &LuaMeshChunks::position,
    //                                     "rotation", &LuaMeshChunks::rotation,
    //                                     "meshes", &LuaMeshChunks::meshes,
    //                                     "cullPriority", &LuaMeshChunks::cullPriority,
    //                                     "setupObject", &LuaMeshChunks::setupObject);

    objects["globalCamera"] = &globalCamera;
    lua["Objects"] = objects;
}