struct Sizes
{
    uint v;
    uint i;
};

#define AIR 0
#define DIRT 1
#define GRASS 2
#define STONE 3
#define WOOD 4
#define LEAVES 5
#define BEDROCK 6

#define NUM_TILES 8.0

static const float2 BEDROCK_TILE = float2(0.0, 7.0/NUM_TILES);
static const float2 LEAVES_TILE = float2(0.0, 6.0/NUM_TILES);
static const float2 WOOD_TOP_TILE = float2(0.0, 5.0/NUM_TILES);
static const float2 WOOD_SIDE_TILE = float2(0.0, 4.0/NUM_TILES);
static const float2 STONE_TILE = float2(0.0, 3.0/NUM_TILES);
static const float2 GRASS_TILE = float2(0.0, 2.0/NUM_TILES);
static const float2 GRASS_SIDE_TILE = float2(0.0, 1.0/NUM_TILES);
static const float2 DIRT_TILE = float2(0.0, 0.00);