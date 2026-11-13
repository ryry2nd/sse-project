#include ".base.slang"
#include ".chunkCommon.slang"

using namespace Structs;
using namespace Normals;
using namespace Utils;
using namespace Random;

BINDING_UAV(0)
RWStructuredBuffer<float> verts;

BINDING_UAV(1)
RWStructuredBuffer<uint> inds;

BINDING_UAV(2)
RWStructuredBuffer<Sizes> sizesout;

BINDING_UAV(3)
RWStructuredBuffer<Uint16> chunkData;

BINDING_UAV(4)
RWStructuredBuffer<float> heightMap;

BINDING_CBV(0)
ConstantBuffer<float> seed;

BINDING_CBV(1)
ConstantBuffer<uint> size;

BINDING_CBV(2)
ConstantBuffer<vec3> chunkOffset;

static const int3 directions[6] =
{
	int3( 1, 0, 0),
	int3(-1, 0, 0),
	int3( 0, 1, 0),
	int3( 0,-1, 0),
	int3( 0, 0, 1),
	int3( 0, 0,-1)
};

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

// --------------------------------------------------
// TERRAIN GENERATION
// --------------------------------------------------

float terrainHeight(int x, int z)
{
	int3 offset = (int3)chunkOffset;

	int localX = x - offset.x;
	int localZ = z - offset.z;

	bool insideChunk =
		localX >= 0 && localX < (int)size &&
		localZ >= 0 && localZ < (int)size;

	if (insideChunk)
	{
		uint index = localZ * size + localX;
		float t = heightMap[index];
		if (t >= 0) {
			return t;
		}
	}

	float2 p = float2(x, z);

	float landMask = smoothstep(0.35f, 0.55f, perlinNoise(p * 0.004, seed));

	float continents = perlinNoise(p * 0.001, seed) * 95.0f * landMask;

	float mountainsM = perlinNoise(p * 0.01, seed + 34.2) * 50.0f;
	float mountainsL = perlinNoise(p * 0.005, seed + 34.3) * 100.0f;

	float mask = perlinNoise(p * 0.0015, seed + 91.7);
	mask = smoothstep(-0.1f, 0.5f, mask);
	mask = smoothstep(0.15f, 0.85f, mask);

	float mountainPresence = smoothstep(-0.1f, 0.4f, perlinNoise(p * 0.0008, seed + 123.4));

	float mountains = mix(mountainsL, mountainsM, mask) * mountainPresence * landMask;


	float hills = perlinNoise(p * 0.045, seed + 37.2) * 2.0f;
	float detail = perlinNoise(p * 0.12, seed + 71.9) * 1.5f;

	float ridges = 1.0 - abs(
		perlinNoise(p * 0.025, seed + 109.3)
	);

	float height = 20.0;

	height += continents;
	// height += mountains;
	// height += hills;
	// height += detail;
	// height += pow(ridges, 2.5) * 14.0;

	height = max(height, 0.0);

	if (insideChunk)
	{
		uint index = localZ * size + localX;
		heightMap[index] = height;
	}

	return height;
}

// --------------------------------------------------
// BLOCK LOOKUP
// --------------------------------------------------
uint hash(int2 p)
{
	uint h = uint(p.x) * 374761393u + uint(p.y) * 668265263u;
	h = (h ^ (h >> 13)) * 1274126177u;
	return h ^ (h >> 16);
}

static const float TREE_DENSITY = 1.0; // Percentage (0-100)

bool isTreePosition(int2 column)
{
	if (terrainHeight(column.x, column.y) < 62) {
		return false;
	}
	uint h = hash(
		column + int2((int)seed, (int)(seed * 127.0))
	);

	float randomValue = (float)(h % 10000u) / 10000.0;

	return randomValue < (TREE_DENSITY / 100.0);
}

Blocks getBlock(int3 worldPos)
{
	if (worldPos.y < 0)
		return Blocks::AIR;

	if (worldPos.y == 0)
		return Blocks::BEDROCK;

	float height = terrainHeight(worldPos.x, worldPos.z);
	int groundY = (int)floor(height);

	// --------------------------------------------------
	// TERRAIN
	// --------------------------------------------------

	if (worldPos.y <= groundY)
	{
		int depth = groundY - worldPos.y;

		if (worldPos.y < 62 && depth <= 3) {
			return Blocks::SAND;
		}
		if (depth == 0)
			return Blocks::GRASS;

		if (depth <= 3)
			return Blocks::DIRT;

		return Blocks::STONE;
	}

	if (worldPos.y < 61) {
		return Blocks::WATER;
	}

	// --------------------------------------------------
	// TREE TRUNK
	// --------------------------------------------------

	int2 column = worldPos.xz;

	uint h = hash(
		column + int2((int)seed, (int)(seed * 127.0))
	);

	if (isTreePosition(column))
	{
		int treeHeight = 4 + int((h >> 8) % 4u);
		int treeTop = groundY + treeHeight;

		if (worldPos.y <= treeTop)
			return Blocks::WOOD;
	}

	// --------------------------------------------------
	// TREE LEAVES
	// --------------------------------------------------

	for (int x = -2; x <= 2; x++)
	{
		for (int z = -2; z <= 2; z++)
		{
			int2 treeColumn = worldPos.xz + int2(x, z);

			uint treeHash = hash(
				treeColumn + int2((int)seed, (int)(seed * 127.0))
			);

			if (!isTreePosition(treeColumn))
				continue;

			int treeGround = (int)floor(
				terrainHeight(treeColumn.x, treeColumn.y)
			);

			int treeHeight = 4 + int((treeHash >> 8) % 4u);
			int treeTop = treeGround + treeHeight;

			int dx = abs(worldPos.x - treeColumn.x);
			int dz = abs(worldPos.z - treeColumn.y);
			int dy = worldPos.y - treeTop;

			// Canopy extends 2 blocks horizontally.
			if (dy >= -2 && dy <= 1)
			{
				int radius = (dy == 1) ? 1 : 2;

				if (dx <= radius && dz <= radius)
				{
					// Trim canopy corners.
					if (dx + dz <= radius + 1)
						return Blocks::LEAVES;
				}
			}
		}
	}

	return Blocks::AIR;
}

// --------------------------------------------------
// MESH GENERATION
// --------------------------------------------------

void writeVertex(
	uint index,
	float3 position,
	float2 uv,
	float3 normal)
{
	uint offset = index * 8;

	verts[offset + 0] = position.x;
	verts[offset + 1] = position.y;
	verts[offset + 2] = position.z;

	verts[offset + 3] = uv.x;
	verts[offset + 4] = uv.y;

	verts[offset + 5] = normal.x;
	verts[offset + 6] = normal.y;
	verts[offset + 7] = normal.z;
}

void generateFace(int3 p, uint face, Blocks block)
{
	uint vertexBase;
	uint indexBase;

	InterlockedAdd(sizesout[0].v, 4, vertexBase);
	InterlockedAdd(sizesout[0].i, 6, indexBase);

	float2 tile = STONE_TILE;

	if (block == Blocks::GRASS)
	{
		tile = (face == 2) ? GRASS_TILE : (face == 3) ? DIRT_TILE : GRASS_SIDE_TILE;
	}
	else if (block == Blocks::DIRT)
	{
		tile = DIRT_TILE;
	}
	else if (block == Blocks::WOOD) {
		tile = (face == 2 || face == 3) ? WOOD_TOP_TILE : WOOD_SIDE_TILE;
	}
	else if (block == Blocks::LEAVES) {
		tile = LEAVES_TILE;
	}
	else if (block == Blocks::SAND)  {
		tile = SAND_TILE;
	}
	else if (block == Blocks::BEDROCK) {
		tile = BEDROCK_TILE;
	}
	else if (block == Blocks::WATER) {
		tile = WATER_TILE;
	}

	#define ROTATION 3

	float2 startUV = tile + float2(0.0, 1.0/NUM_TILES);
	float2 endUV = tile + float2(1.0, 0.0);

	float2 uv0 = lerp(startUV, endUV, rotateUV(float2(0, 1), ROTATION));
	float2 uv1 = lerp(startUV, endUV, rotateUV(float2(1, 1), ROTATION));
	float2 uv2 = lerp(startUV, endUV, rotateUV(float2(1, 0), ROTATION));
	float2 uv3 = lerp(startUV, endUV, rotateUV(float2(0, 0), ROTATION));

	float3 base = float3(p);

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

// --------------------------------------------------
// COMPUTE SHADER
// --------------------------------------------------

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

	int3 offset = (int3)chunkOffset;
	int3 worldPos = p + offset;

	// Get and store this voxel's block.
	Blocks block = getBlock(worldPos);

	chunkData[index] = block;

	// Air has no geometry.
	if (block == Blocks::AIR)
		return;

	// Check all six neighboring blocks.
	for (uint face = 0; face < 6; face++)
	{
		int3 neighborPos = worldPos + directions[face];

		Blocks neighborBlock = getBlock(neighborPos);

		// Generate faces against air.
		// Also generate opaque block faces against leaves,
		// so opaque blocks remain visible through transparent leaf pixels.
		bool shouldGenerateFace =
			neighborBlock == Blocks::AIR ||
			(block != Blocks::LEAVES && neighborBlock == Blocks::LEAVES) ||
			(block != Blocks::WATER && neighborBlock == Blocks::WATER);

		if (shouldGenerateFace)
		{
			generateFace(p, face, block);
		}
	}
}