#include <spdlog/spdlog.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "Mesh.hpp"

using namespace Engine::Rendering;

void Mesh::getObj(const char *fname, std::vector<Vert> &verts, std::vector<Ind> &inds)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool ok = tinyobj::LoadObj(
        &attrib, &shapes, &materials, &warn, &err, fname
    );

    if (!ok) {
        spdlog::error("OBJ loader: {}", err);
        return;
    }

    verts.clear();
    inds.clear();

    for (const auto& shape : shapes) {
        for (const auto& idx : shape.mesh.indices) {
            Vert v{};

            v.pos = {
                attrib.vertices[3 * idx.vertex_index + 0],
                attrib.vertices[3 * idx.vertex_index + 1],
                attrib.vertices[3 * idx.vertex_index + 2]
            };

            if (idx.texcoord_index >= 0) {
                v.tex = {
                    attrib.texcoords[2 * idx.texcoord_index + 0],
                    attrib.texcoords[2 * idx.texcoord_index + 1]
                };
            } else {
                v.tex = { 0.0f, 0.0f };
            }

            if (idx.normal_index >= 0) {
                v.nor = {
                    attrib.normals[3 * idx.normal_index + 0],
                    attrib.normals[3 * idx.normal_index + 1],
                    attrib.normals[3 * idx.normal_index + 2]
                };
            } else {
                v.nor = { 0.0f, 0.0f, 0.0f };
            }

            inds.push_back((Ind)verts.size());
            verts.push_back(v);
        }
    }
}