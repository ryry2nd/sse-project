#include <spdlog/spdlog.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "Mesh.hpp"

using namespace Engine::Rendering;

namespace {
struct Key {
    int v, t, n;
    bool operator==(const Key& o) const { return v == o.v && t == o.t && n == o.n; }
};
struct KeyHash {
    size_t operator()(const Key& k) const {
        size_t h = (size_t)k.v * 73856093u;
        h ^= (size_t)k.t * 19349663u;
        h ^= (size_t)k.n * 83492791u;
        return h;
    }
};
}

void Mesh::getObj(const char *fname, std::vector<Vert> &verts, std::vector<Ind> &inds) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	bool ok = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, fname);
	if (!ok) {
		spdlog::error("OBJ loader: {}", err);
		return;
	}

	verts.clear();
	inds.clear();

	std::unordered_map<Key, Ind, KeyHash> unique;

	for (const auto& shape : shapes) {
        for (const auto& idx : shape.mesh.indices) {
            Key key{ idx.vertex_index, idx.texcoord_index, idx.normal_index };

            auto it = unique.find(key);
            if (it != unique.end()) {
                inds.push_back(it->second);
                continue;
            }

            Vert v{};
            v.pos = {
                attrib.vertices[3 * idx.vertex_index + 0],
                attrib.vertices[3 * idx.vertex_index + 1],
                attrib.vertices[3 * idx.vertex_index + 2]
            };

            if (idx.texcoord_index >= 0) {
                v.tex = {
                    attrib.texcoords[2 * idx.texcoord_index + 0],
                    attrib.texcoords[2 * idx.texcoord_index + 1]   // use 1.0f - y if your API has a top-left UV origin
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

            Ind newIndex = (Ind)verts.size();
            verts.push_back(v);
            unique.emplace(key, newIndex);
            inds.push_back(newIndex);
        }
    }
}