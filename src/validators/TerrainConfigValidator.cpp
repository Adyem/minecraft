#include "../../src/validators/TerrainConfigValidator.hpp"

TerrainConfigValidator::TerrainConfigValidator() {}
TerrainConfigValidator::TerrainConfigValidator(const TerrainConfigValidator &other)
{
    (void)other;
}
TerrainConfigValidator::~TerrainConfigValidator() {}
TerrainConfigValidator &TerrainConfigValidator::operator=(const TerrainConfigValidator &other)
{
    (void)other;
    return (*this);
}

int TerrainConfigValidator::validate() const
{
    game_voxel_chunk chunk;
    terrain_generation_config config = terrain_default_generation_config();
    uint32_t block_id;

    config.biome_count = 1U;
    config.sea_level = 0;
    config.water_chance_percent = 0U;
    config.biomes[0].profile.surface_height = 32;
    config.biomes[0].profile.height_variation = 0;
    config.biomes[0].profile.topsoil_depth = 0;
    config.biomes[0].surface_block_id = TERRAIN_GENERATOR_SAND_BLOCK;
    config.biomes[0].allow_shrubs = FT_FALSE;
    config.biomes[0].allow_trees = FT_FALSE;
    if (chunk.initialize() != FT_ERR_SUCCESS)
        return (1);
    if (terrain_generate_chunk(chunk, 0, 0, "config-validator", config)
        != FT_ERR_SUCCESS)
    {
        (void)chunk.destroy();
        return (1);
    }
    if (chunk.read_block(0, 32, 0, &block_id) != FT_ERR_SUCCESS
        || block_id != TERRAIN_GENERATOR_SAND_BLOCK)
    {
        (void)chunk.destroy();
        return (1);
    }
    (void)chunk.destroy();
    return (0);
}
