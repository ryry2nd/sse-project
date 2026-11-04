#include ".base.slang"

using namespace Structs;
using namespace Normals;
using namespace Utils;
using namespace Random;

BINDING_UAV(0)
RWStructuredBuffer<float> verts;

BINDING_CBV(0)
ConstantBuffer<uint> vertexCount;


float generateNoise(float x, float z)
{
    float2 p = float2(x, z);
    float seed = 42.0;

    float large = perlinNoise(p * 0.003, seed);

    float ridge1 = 1.0 - abs(perlinNoise(p * 0.008, seed + 1.0));
    float ridge2 = 1.0 - abs(perlinNoise(p * 0.02, seed + 2.0));
    float ridge3 = 1.0 - abs(perlinNoise(p * 0.05, seed + 3.0));

    float height = max(large, 0.0) * 4.0;
    height += pow(ridge1, 3.0) * 18.0;
    height += pow(ridge2, 3.0) * 4.0;
    height += ridge3 * 1.0;

    return height;
}

[shader("compute")]
[numthreads(256, 1, 1)]
void computeMain(uint3 id : SV_DispatchThreadID)
{
    uint index = id.x;

    if (index >= vertexCount)
        return;

    uint base = index * 8;

    float x = verts[base + 0];
    float z = verts[base + 2];

    verts[base + 1] = generateNoise(x, z);
}