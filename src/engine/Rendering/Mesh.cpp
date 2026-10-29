
#include <spdlog/spdlog.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "Mesh.hpp"

#include <utility>
#include <vector>

using namespace Engine::Rendering;

void Mesh::getObj(const char* fname, std::vector<Shape>& shapesout)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> objShapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    const bool ok = tinyobj::LoadObj(
        &attrib,
        &objShapes,
        &materials,
        &warn,
        &err,
        fname
    );

    if (!warn.empty()) {
        spdlog::warn("OBJ Warning: {}", warn);
    }

    if (!ok) {
        spdlog::error("OBJ Error: {}", err);
        return;
    }

    // Build the result separately. Do not destroy the existing output
    // until the new mesh has been validated and constructed.
    std::vector<Shape> loaded;
    loaded.reserve(objShapes.size());

    for (const auto& objShape : objShapes) {
        const auto& mesh = objShape.mesh;

        // Reject malformed vertex data before indexing any attribute arrays.
        for (const auto& idx : mesh.indices) {
            if (idx.vertex_index < 0 ||
                static_cast<size_t>(idx.vertex_index) >=
                    attrib.vertices.size() / 3) {
                spdlog::error("OBJ Error: Invalid vertex index in shape '{}'",
                              objShape.name);
                return;
            }

            if (idx.texcoord_index >= 0 &&
                static_cast<size_t>(idx.texcoord_index) >=
                    attrib.texcoords.size() / 2) {
                spdlog::error("OBJ Error: Invalid texture coordinate index "
                              "in shape '{}'", objShape.name);
                return;
            }

            if (idx.normal_index >= 0 &&
                static_cast<size_t>(idx.normal_index) >=
                    attrib.normals.size() / 3) {
                spdlog::error("OBJ Error: Invalid normal index in shape '{}'",
                              objShape.name);
                return;
            }
        }

        Shape out{};

        out.verts.resize(mesh.indices.size());
        out.inds.resize(mesh.indices.size());

        // Copy material properties from the first face's material.
        if (!mesh.material_ids.empty() &&
            mesh.material_ids[0] >= 0 &&
            static_cast<size_t>(mesh.material_ids[0]) < materials.size()) {

            const auto& mat = materials[mesh.material_ids[0]];

            out.ambient = {
                mat.ambient[0], mat.ambient[1], mat.ambient[2]
            };

            out.diffuse = {
                mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]
            };

            out.specular = {
                mat.specular[0], mat.specular[1], mat.specular[2]
            };

            out.emissive = {
                mat.emission[0], mat.emission[1], mat.emission[2]
            };

            out.shininess = mat.shininess;
            out.opacity = mat.dissolve;
            out.ior = mat.ior;

            out.ambientTex = mat.ambient_texname;
            out.diffuseTex = mat.diffuse_texname;
            out.specularTex = mat.specular_texname;
            out.emissiveTex = mat.emissive_texname;
            out.shininessTex = mat.specular_highlight_texname;
            out.opacityTex = mat.alpha_texname;
            out.bumpTex = mat.bump_texname;
            out.normalTex = mat.normal_texname;
            out.displacementTex = mat.displacement_texname;
        }

        for (size_t i = 0; i < mesh.indices.size(); ++i) {
            const auto& idx = mesh.indices[i];
            Vert v{};

            const size_t vi = static_cast<size_t>(idx.vertex_index) * 3;

            v.pos = {
                attrib.vertices[vi],
                attrib.vertices[vi + 1],
                attrib.vertices[vi + 2]
            };

            if (idx.texcoord_index >= 0) {
                const size_t ti =
                    static_cast<size_t>(idx.texcoord_index) * 2;

                v.tex = {
                    attrib.texcoords[ti],
                    attrib.texcoords[ti + 1]
                };
            } else {
                v.tex = { 0.0f, 0.0f };
            }

            if (idx.normal_index >= 0) {
                const size_t ni =
                    static_cast<size_t>(idx.normal_index) * 3;

                v.nor = {
                    attrib.normals[ni],
                    attrib.normals[ni + 1],
                    attrib.normals[ni + 2]
                };
            } else {
                v.nor = { 0.0f, 0.0f, 0.0f };
            }

            out.verts[i] = v;
            out.inds[i] = static_cast<Ind>(i);
        }

        loaded.push_back(std::move(out));
    }

    // Replace output only after successful construction.
    shapesout = std::move(loaded);
}