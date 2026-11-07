struct Sizes
{
    uint v;
    uint i;
};

enum Blocks : uint16_t
{
    AIR = 0,
    DIRT = 1,
    GRASS = 2,
    STONE = 3,
	WOOD = 4,
	LEAVES = 5,
	BEDROCK = 6,
	SAND = 7,
	WATER = 8,
};

#define NUM_TILES 10.0

static const float2 WATER_TILE = float2(0.0, 9.0/NUM_TILES);
static const float2 SAND_TILE = float2(0.0, 8.0/NUM_TILES);
static const float2 BEDROCK_TILE = float2(0.0, 7.0/NUM_TILES);
static const float2 LEAVES_TILE = float2(0.0, 6.0/NUM_TILES);
static const float2 WOOD_TOP_TILE = float2(0.0, 5.0/NUM_TILES);
static const float2 WOOD_SIDE_TILE = float2(0.0, 4.0/NUM_TILES);
static const float2 STONE_TILE = float2(0.0, 3.0/NUM_TILES);
static const float2 GRASS_TILE = float2(0.0, 2.0/NUM_TILES);
static const float2 GRASS_SIDE_TILE = float2(0.0, 1.0/NUM_TILES);
static const float2 DIRT_TILE = float2(0.0, 0.00);