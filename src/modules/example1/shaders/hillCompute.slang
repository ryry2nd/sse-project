#include ".base.slang"

using namespace Structs;
using namespace Normals;
using namespace Utils;
using namespace Random;

struct Sizes
{
    uint v;
    uint i;
};

BINDING_UAV(0)
RWStructuredBuffer<float> verts;

BINDING_UAV(1)
RWStructuredBuffer<uint> inds;

BINDING_UAV(2)
RWStructuredBuffer<Sizes> sizesout;

BINDING_CBV(0)
ConstantBuffer<float> seed;

BINDING_CBV(1)
ConstantBuffer<uint> size;

static const float2 ATLAS_TILE_SIZE = float2(1.0, 0.25);

static const float2 STONE_TILE = float2(0.0, 0.75);
static const float2 GRASS_TILE  = float2(0.0, 0.50);
static const float2 GRASS_SIDE_TILE = float2(0.0, 0.25);
static const float2 DIRT_TILE  = float2(0.0, 0.00);

#define DIRT 0
#define GRASS 1
#define STONE 2

static const int3 directions[6] =
{
    int3( 1, 0, 0),
    int3(-1, 0, 0),
    int3( 0, 1, 0),
    int3( 0,-1, 0),
    int3( 0, 0, 1),
    int3( 0, 0,-1)
};

// Four corners per cube face, outward-facing winding.
static const float3 corners[6][4] =
{
    {
        float3(1,0,0), float3(1,1,0),
        float3(1,1,1), float3(1,0,1)
    },
    {
        float3(0,0,1), float3(0,1,1),
        float3(0,1,0), float3(0,0,0)
    },
    {
        float3(0,1,1), float3(1,1,1),
        float3(1,1,0), float3(0,1,0)
    },
    {
        float3(0,0,0), float3(1,0,0),
        float3(1,0,1), float3(0,0,1)
    },
    {
        float3(1,0,1), float3(1,1,1),
        float3(0,1,1), float3(0,0,1)
    },
    {
        float3(0,0,0), float3(0,1,0),
        float3(1,1,0), float3(1,0,0)
    }
};

static const float3 normals[6] =
{
    float3( 1, 0, 0),
    float3(-1, 0, 0),
    float3( 0, 1, 0),
    float3( 0,-1, 0),
    float3( 0, 0, 1),
    float3( 0, 0,-1)
};

float terrainHeight(int x, int z)
{
    float2 p = float2(x, z);

    // Large-scale continental shape
    float continental = perlinNoise(
        p * 0.008,
        seed
    );

    // Mountain ranges and large hills
    float mountains = perlinNoise(
        p * 0.018,
        seed + 13.7
    );

    // Medium-scale hills
    float hills = perlinNoise(
        p * 0.045,
        seed + 37.2
    );

    // Small-scale surface variation
    float detail = perlinNoise(
        p * 0.12,
        seed + 71.9
    );

    // Ridged noise creates sharp mountain peaks
    float ridges = 1.0 - abs(perlinNoise(
        p * 0.025,
        seed + 109.3
    ));

    // Combine terrain layers
    float height = 16.0;

    height += continental * 13.0;
    height += mountains * 18.0;
    height += hills * 4.0;
    height += detail * 1.5;

    // Add mountain ridges
    height += pow(ridges, 2.5) * 14.0;

    return clamp(height, 1.0, 31.0);
}

bool isSolid(int3 p)
{
    if (p.y < 0)
        return true;

    if (p.y >= size)
        return false;

	float height = terrainHeight(p.x, p.z);

    return float(p.y) <= height;
}

void writeVertex(
    uint index,
    float3 position,
    float2 uv,
    float3 normal)
{
    uint offset = index * 8;

    // Position
    verts[offset + 0] = position.x;
    verts[offset + 1] = position.y;
    verts[offset + 2] = position.z;

    // UV
    verts[offset + 3] = uv.x;
    verts[offset + 4] = uv.y;

    // Normal
    verts[offset + 5] = normal.x;
    verts[offset + 6] = normal.y;
    verts[offset + 7] = normal.z;
}

void generateFace(int3 p, uint face)
{
    uint vertexBase;
    uint indexBase;

    InterlockedAdd(sizesout[0].v, 4, vertexBase);
    InterlockedAdd(sizesout[0].i, 6, indexBase);

    float3 base = float3(p);
	vec2 tile;

	float h = terrainHeight(p.x, p.z);
	int sub = (int)floor(h) - p.y;

	if (sub == 0)
	{
		tile = (face == 2) ? GRASS_TILE : GRASS_SIDE_TILE;
	}
	else if (sub == 1)
	{
		tile = DIRT_TILE;
	}
	else
	{
		tile = STONE_TILE;
	}

	#define ROTATION 3

	float2 startUV = tile + float2(0.0, 0.25);
	float2 endUV   = tile + float2(1.0, 0.0);

	float2 uv0 = lerp(startUV, endUV, rotateUV(float2(0, 1), ROTATION));
	float2 uv1 = lerp(startUV, endUV, rotateUV(float2(1, 1), ROTATION));
	float2 uv2 = lerp(startUV, endUV, rotateUV(float2(1, 0), ROTATION));
	float2 uv3 = lerp(startUV, endUV, rotateUV(float2(0, 0), ROTATION));

    writeVertex(vertexBase + 0, base + corners[face][0], uv0, normals[face]);
	writeVertex(vertexBase + 1, base + corners[face][1], uv1, normals[face]);
	writeVertex(vertexBase + 2, base + corners[face][2], uv2, normals[face]);
	writeVertex(vertexBase + 3, base + corners[face][3], uv3, normals[face]);

    inds[indexBase + 0] = vertexBase + 0;
    inds[indexBase + 1] = vertexBase + 1;
    inds[indexBase + 2] = vertexBase + 2;

    inds[indexBase + 3] = vertexBase + 0;
    inds[indexBase + 4] = vertexBase + 2;
    inds[indexBase + 5] = vertexBase + 3;
}

[shader("compute")]
[numthreads(256, 1, 1)]
void computeMain(uint3 id : SV_DispatchThreadID)
{
    uint index = id.x;

    if (index >= size * size * size)
        return;

    int3 p = int3(
        index % size,
        (index / size) % size,
        index / (size * size)
    );

    if (!isSolid(p))
        return;

    for (uint face = 0; face < 6; face++)
    {
        if (!isSolid(p + directions[face]))
            generateFace(p, face);
    }
}