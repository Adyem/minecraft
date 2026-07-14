#include "../../src/assets/BlockTextureRegistry.hpp"

BlockTextureRegistry::BlockTextureRegistry()
{
}
BlockTextureRegistry::BlockTextureRegistry(const BlockTextureRegistry &o)
{
    (void)o;
}
BlockTextureRegistry::~BlockTextureRegistry()
{
}
BlockTextureRegistry &BlockTextureRegistry::operator=(const BlockTextureRegistry &o)
{
    (void)o;
    return *this;
}

const int BlockTextureRegistry::DEFAULT_X = 2;
const int BlockTextureRegistry::DEFAULT_Y = 2;

const BlockTextureRegistry::Entry BlockTextureRegistry::ENTRIES[] = {
    {TERRAIN_GENERATOR_GRASS_BLOCK, 1, 0, 0},    {TERRAIN_GENERATOR_DIRT_BLOCK, 2, 0, -1},
    {TERRAIN_GENERATOR_STONE_BLOCK, 3, 0, -1},   {TERRAIN_GENERATOR_SHRUB_BLOCK, 1, 2, -1},
    {TERRAIN_GENERATOR_OAK_LOG_BLOCK, 2, 1, -1}, {TERRAIN_GENERATOR_OAK_LEAVES_BLOCK, 3, 1, -1},
    {TERRAIN_GENERATOR_CACTUS_BLOCK, 0, 2, -1},  {TERRAIN_GENERATOR_WATER_BLOCK, 0, 1, -1},
    {TERRAIN_GENERATOR_BEDROCK_BLOCK, 2, 2, -1}, {TERRAIN_GENERATOR_SAND_BLOCK, 1, 3, -1},
    {TERRAIN_GENERATOR_SNOW_BLOCK, 1, 1, -1},    {TERRAIN_GENERATOR_PERMAFROST_BLOCK, 3, 2, -1},
    {TERRAIN_GENERATOR_CANYON_ROCK_BLOCK, 2, 3, -1}, {TERRAIN_GENERATOR_SLATE_BLOCK, 3, 3, -1},
    {TERRAIN_GENERATOR_MOSS_ROCK_BLOCK, 0, 3, -1},
};

const int BlockTextureRegistry::ENTRY_COUNT =
    static_cast<int>(sizeof(ENTRIES) / sizeof(ENTRIES[0]));

void BlockTextureRegistry::tile_for_block(uint32_t block_id, uint8_t face, int *tile_x, int *tile_y)
{
    if (!tile_x || !tile_y)
        return;
    *tile_x = DEFAULT_X;
    *tile_y = DEFAULT_Y;
    for (int i = 0; i < ENTRY_COUNT; ++i)
    {
        if (ENTRIES[i].block_id != block_id)
            continue;
        *tile_y = ENTRIES[i].tile_y;
        if (face == CHUNK_MESH_FACE_UP && ENTRIES[i].up_x >= 0)
            *tile_x = ENTRIES[i].up_x;
        else
            *tile_x = ENTRIES[i].def_x;
        return;
    }
}
