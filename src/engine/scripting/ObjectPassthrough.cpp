#include "ScriptingHeaders.hpp"

// #include "../objects/Objects.hpp"
// #include "../customMath/Bigint.hpp"
// #include "../customMath/BigVec.hpp"
// #include "../rendering/base/HelperFunctions.hpp"

using namespace ScriptingHeaders;

// struct LuaRenderObject : public Objects::RenderObject
// {
//     sol::table lua_instance;

//     LuaRenderObject() = default;
//     LuaRenderObject(Rendering::Shader *shader, Rendering::Image *image)
//         : Objects::RenderObject(shader) {}

//     void appendUpdate(const float &deltaTime) override
//     {
//         if (!lua_instance.valid())
//         {
//             Objects::RenderObject::appendUpdate(deltaTime);
//             return;
//         }

//         sol::function override = lua_instance["appendUpdate"];
//         if (override.valid())
//         {
//             sol::protected_function_result result = override(lua_instance, deltaTime);
//             if (!result.valid())
//             {
//                 sol::error err = result;
//                 std::cerr << "[C++] Lua error in appendUpdate: " << err.what() << "\n";
//             }
//         }
//         else
//         {
//             Objects::RenderObject::appendUpdate(deltaTime);
//         }
//     }

//     using Objects::RenderObject::meshes;
//     using Objects::RenderObject::setupObject;
// };

// struct LuaRenderObject2d : public Objects::RenderObject2d
// {
//     sol::table lua_instance;
//     LuaRenderObject2d(Rendering::Shader *shader, Rendering::Image *im)
//         : Objects::RenderObject2d(shader, im) {}

//     void Update(const float &deltaTime) override
//     {
//         if (!lua_instance.valid())
//         {
//             Objects::RenderObject2d::Update(deltaTime);
//             return;
//         }

//         sol::function override = lua_instance["Update"];
//         if (override.valid())
//         {
//             sol::protected_function_result result = override(lua_instance, deltaTime);
//             if (!result.valid())
//             {
//                 sol::error err = result;
//                 std::cerr << "[C++] Lua error in Update: " << err.what() << "\n";
//             }
//         }
//         else
//         {
//             Objects::RenderObject2d::Update(deltaTime);
//         }
//     }
// };
// struct LuaMeshChunks : public Objects::MeshChunks
// {
//     LuaMeshChunks(Rendering::Shader *shader, Rendering::Image *image)
//         : Objects::MeshChunks(shader, image) {}
//     using Objects::MeshChunks::meshes;
//     using Objects::MeshChunks::setupObject;
// };

void Package::includeGlm()
{
    // sol::table vecs = lua.create_table();
    // // vecs.set_function("length", &glm::length);
    // // vecs.set_function("normalize", &glm::normalize);
    // vecs.new_usertype<glm::vec2>("vec2", sol::constructors<glm::vec2(), glm::vec2(float, float)>(), "x", &glm::vec2::x, "y", &glm::vec2::y, sol::meta_function::addition, [](const glm::vec2 &a, const glm::vec2 &b)
    //                              { return a + b; }, sol::meta_function::subtraction, [](const glm::vec2 &a, const glm::vec2 &b)
    //                              { return a - b; }, sol::meta_function::multiplication, [](const glm::vec2 &a, float b)
    //                              { return a * b; }, // vector * scalar
    //                              sol::meta_function::division, [](const glm::vec2 &a, float b)
    //                              { return a / b; },

    //                              sol::meta_function::equal_to, [](const glm::vec2 &a, const glm::vec2 &b)
    //                              { return a == b; });
    // vecs.new_usertype<glm::vec3>("vec3", sol::constructors<glm::vec3(), glm::vec3(float, float, float)>(), "x", &glm::vec3::x, "y", &glm::vec3::y, "z", &glm::vec3::z, sol::meta_function::addition, [](const glm::vec3 &a, const glm::vec3 &b)
    //                              { return a + b; }, sol::meta_function::subtraction, [](const glm::vec3 &a, const glm::vec3 &b)
    //                              { return a - b; }, sol::meta_function::multiplication, [](const glm::vec3 &a, float b)
    //                              { return a * b; }, // vector * scalar
    //                              sol::meta_function::division, [](const glm::vec3 &a, float b)
    //                              { return a / b; },

    //                              sol::meta_function::equal_to, [](const glm::vec3 &a, const glm::vec3 &b)
    //                              { return a == b; });
    // lua["glm"] = vecs;
}

void Package::includeBigVars()
{
    // sol::table bigvars = lua.create_table();
    // bigvars.new_usertype<Bigint>("Bigint",
    //                              sol::constructors<Bigint(), Bigint(int), Bigint(float), Bigint(double), Bigint(long), Bigint(std::string)>(),
    //                              "isZero", &Bigint::isZero, "getAbs", &Bigint::getAbs, "sqrt", &Bigint::sqrt, "toDouble", &Bigint::toDouble, "toFloat", &Bigint::toFloat, "toString", &Bigint::toString, "getHoweverManyDigits", &Bigint::getHoweverManyDigits,
    //                              // Operator overloading
    //                              sol::meta_function::addition, &Bigint::operator+,
    //                              sol::meta_function::subtraction, &Bigint::operator-,
    //                              sol::meta_function::multiplication, &Bigint::operator*,
    //                              sol::meta_function::division, &Bigint::operator/,
    //                              sol::meta_function::less_than, &Bigint::operator<,
    //                              sol::meta_function::less_than_or_equal_to, &Bigint::operator<=,
    //                              sol::meta_function::equal_to, &Bigint::operator==,
    //                              sol::meta_function::to_string, &Bigint::toString);

    // bigvars.new_usertype<BigVec3>("BigVec3", sol::constructors<BigVec3(), BigVec3(glm::vec3), BigVec3(Bigint), BigVec3(Bigint, Bigint, Bigint)>(), "getMaxAbs", &BigVec3::getMaxAbs, "getMinAbs", &BigVec3::getMinAbs, "isZero", &BigVec3::isZero, "x", &BigVec3::x, "y", &BigVec3::y, "z", &BigVec3::z,
    //                               // Meta operator overloads
    //                               sol::meta_function::addition, [](const BigVec3 &a, const BigVec3 &b)
    //                               { return a + b; }, sol::meta_function::subtraction, [](const BigVec3 &a, const BigVec3 &b)
    //                               { return a - b; }, sol::meta_function::multiplication, [](const BigVec3 &a, const BigVec3 &b)
    //                               { return a * b; }, sol::meta_function::division, [](const BigVec3 &a, const BigVec3 &b)
    //                               { return a / b; });

    // lua["bigvars"] = bigvars;
}

void Package::includeRendering()
{
    // using namespace Rendering;
    // sol::table rendering = lua.create_table();

    // rendering.new_usertype<HelperFunctions>("HelperFunctions", sol::no_constructor,
    //                                         "deltaTime", sol::property([]()
    //                                                                    { return Rendering::HelperFunctions::deltaTime; }),
    //                                         "fps", sol::property([]()
    //                                                              { return Rendering::HelperFunctions::fps; }));
    // rendering.new_usertype<Font>("Font", sol::no_constructor);
    // rendering.new_usertype<Shader>("Shader", sol::no_constructor); //, "createUniform", &Shader::createUniform, "includeShader", &Shader::includeShader, "setUniform", &Shader::setUniform, "enableCulling", &Shader::enableCulling, "disableCulling", &Shader::disableCulling);
    // rendering.new_usertype<Image>("Image", sol::no_constructor, "imageSizes", &Image::imageSizes);
    // rendering.new_usertype<Mesh>("Mesh", sol::no_constructor, "posOffset", &Mesh::posOffset, "rotOffset", &Mesh::rotOffset, "sizeOffset", &Mesh::sizeOffset);
    // lua["Rendering"] = rendering;
}

void Package::includeObjects()
{
    // using namespace Objects;
    // sol::table objects = lua.create_table();

    // objects.new_usertype<BaseObject>("BaseObject", sol::constructors<BaseObject(), BaseObject(const BigVec3 &, const glm::vec3 &)>(),
    //                                  "position", &BaseObject::position, "rotation", &BaseObject::rotation,
    //                                  "children", &BaseObject::children, "getTruePos", &BaseObject::getTruePos,
    //                                  "makeParent", &BaseObject::makeParent, "parent", &BaseObject::parent, "removeParent", &BaseObject::removeParent);
    // objects.new_usertype<Camera>("Camera", sol::constructors<Camera(), Camera(BigVec3 &, glm::vec3 &)>(),
    //                              sol::base_classes, sol::bases<BaseObject>(),
    //                              "fov", &Camera::fov, "near", &Camera::near, "far", &Camera::far,
    //                              "convertToLocal", &Camera::convertToLocal, "getViewMatrix", &Camera::getViewMatrix,
    //                              "getProjectionMatrix", &Camera::getProjectionMatrix, "getViewMatrix2d", &Camera::getProjectionMatrix2d,
    //                              "getDownVector", &Camera::getDownVector, "getForwardVector", &Camera::getForwardVector, "getRightVector", &Camera::getRightVector);
    // objects.new_usertype<LuaRenderObject>("RenderObject",
    //                                       sol::constructors<LuaRenderObject(), LuaRenderObject(Rendering::Shader *, Rendering::Shader *, Rendering::Image *)>(),
    //                                       sol::base_classes, sol::bases<BaseObject>(),
    //                                       "lua_instance", &LuaRenderObject::lua_instance,
    //                                       "meshes", &LuaRenderObject::meshes,
    //                                       "appendUpdate", &LuaRenderObject::appendUpdate,
    //                                       "setupObject", &LuaRenderObject::setupObject);
    // objects.new_usertype<LuaRenderObject2d>("RenderObject2d", sol::constructors<LuaRenderObject2d(Rendering::Shader *, Rendering::Image *)>(),
    //                                         "position", &LuaRenderObject2d::position, "rotation", &LuaRenderObject2d::rotation, "scale", &LuaRenderObject2d::scale,
    //                                         "Update", &LuaRenderObject2d::Update, "lua_instance", &LuaRenderObject2d::lua_instance);
    // // objects.new_usertype<TextRenderObject>("TextRenderObject", sol::constructors<TextRenderObject(Rendering::Shader *, Rendering::Font *, const std::string &, const glm::vec4 &)>(),
    // //             sol::base_classes, sol::bases<LuaRenderObject2d>(),
    // //             "message", &TextRenderObject::message,
    // //             "font_color", &TextRenderObject::font_color,
    // //             "changeText", &TextRenderObject::changeText
    // //             );

    // objects["globalCamera"] = &globalCamera;
    // lua["Objects"] = objects;
}