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
    terrain_generation_config config;

    terrain_default_generation_config(config);
    uint32_t block_id;

    config.set_biome_count(1U);
    config.set_sea_level(0);
    config.set_water_chance_percent(0U);
    terrain_biome_profile profile;
    profile.surface_height = 32;
    profile.height_variation = 0;
    profile.topsoil_depth = 0;
    config.biomes[0].set_profile(profile);
    config.biomes[0].set_block_palette(TERRAIN_GENERATOR_SAND_BLOCK,
        TERRAIN_GENERATOR_SAND_BLOCK, TERRAIN_GENERATOR_STONE_BLOCK);
    config.biomes[0].set_decoration_policy(FT_FALSE, FT_FALSE, 6U, 18U);
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
