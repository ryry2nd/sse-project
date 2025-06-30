#include "Objects.hpp"

using namespace Objects;

MeshChunks::MeshChunks(Shader *shader, Image *image)
{
    //     setupObject(shader, image);
    //
    //     float size = 512.0f;
    //     float s = 0.5f;
    //     glm::vec2 grid(2, 2);
    //     int x, y, z, i;
    //     float fx, fy, fz;
    //
    //     std::vector<float> bigMesh;
    //     std::vector<float> tempVerts;
    //     for (x = 0; x < size; x++)
    //     {
    //         for (y = 0; y < size; y++)
    //         {
    //             for (z = 0; z < size; z++)
    //             {
    //                 tempVerts.clear();
    //                 fx = s * x * 2 - size;
    //                 fy = s * y * 2 - size;
    //                 fz = s * z * 2 - size;
    //                 // // Front face (+Z)
    //                 // if (z == size - 1)
    //                 //     addFace(tempVerts, {fx - s, fy - s, fz + s}, {fx + s, fy - s, fz + s}, {fx + s, fy + s, fz + s}, {fx - s, fy + s, fz + s}, {0.0f, 0.0f}, {1.0f, 1.0f});
    //                 // // Back face (-Z)
    //                 // if (z == 0)
    //                 //     addFace(tempVerts, {fx + s, fy - s, fz - s}, {fx - s, fy - s, fz - s}, {fx - s, fy + s, fz - s}, {fx + s, fy + s, fz - s}, {0.0f, 0.0f}, {1.0f, 1.0f});
    //                 // // Left face (-X)
    //                 // if (x == 0)
    //                 //     addFace(tempVerts, {fx - s, fy - s, fz - s}, {fx - s, fy - s, fz + s}, {fx - s, fy + s, fz + s}, {fx - s, fy + s, fz - s}, {0.0f, 0.0f}, {1.0f, 1.0f});
    //                 // // Right face (+X)
    //                 // if (x == size - 1)
    //                 //     addFace(tempVerts, {fx + s, fy - s, fz + s}, {fx + s, fy - s, fz - s}, {fx + s, fy + s, fz - s}, {fx + s, fy + s, fz + s}, {0.0f, 0.0f}, {1.0f, 1.0f});
    //                 // Top face (+Y)
    //                 if (y == size - 1)
    //                     makeFace(tempVerts, {fx - s, fy + s, fz + s}, {fx + s, fy + s, fz + s}, {fx + s, fy + s, fz - s}, {fx - s, fy + s, fz - s}, {0.0f, 0.0f}, {1.0f, 1.0f});
    //                 // Bottom face (-Y)
    //                 if (y == 0)
    //                     addFace(tempVerts, {fx - s, fy - s, fz - s}, {fx + s, fy - s, fz - s}, {fx + s, fy - s, fz + s}, {fx - s, fy - s, fz + s}, {0.0f, 0.0f}, {1.0f, 1.0f});
    //
    //                 bigMesh.insert(bigMesh.end(), tempVerts.begin(), tempVerts.end());
    //             }
    //         }
    //     }
    //
    //     Mesh *tempMesh;
    //     for (x = 0; x < grid.x; x++)
    //     {
    //         for (y = 0; y < grid.y; y++)
    //         {
    //             tempMesh = defaultMeshAPI->makeNewMesh(bigMesh, {3, 2, 3});
    //             tempMesh->posOffset = glm::vec3(x * size, 0, y * size);
    //             // tempMesh->sizeOffset = glm::vec3(size);
    //             meshes.push_back(tempMesh);
    //         }
    //     }
}

void MeshChunks::appendUpdate(const float &deltaTime) {}