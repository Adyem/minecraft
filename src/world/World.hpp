#ifndef WORLD_HPP
#define WORLD_HPP

#include "../ft_vox.hpp"
#include "../../src/chunks/WorldChunk.hpp"
#include "../../src/chunks/WorldChunkLoader.hpp"
#include "../../src/chunks/WorldChunkStore.hpp"
#include "../../src/coordinates/WorldCoordinates.hpp"
#include "../../Libft/Modules/Errno/errno.hpp"
#include "../../Libft/Modules/Voxel/voxel.hpp"
#include "../../src/queries/WorldBlockQuery.hpp"
#include "../../src/edits/WorldBlockEditor.hpp"
#include "../../src/queries/WorldRaycaster.hpp"
#include "../../src/validators/WorldVisibilityValidator.hpp"

class World
{
  public:
    WorldChunk chunks[WorldCoordinates::CHUNK_COUNT];
    WorldChunk *chunk_index[WorldCoordinates::CHUNK_COUNT];
    int32_t chunk_index_center_x;
    int32_t chunk_index_center_z;
    bool chunk_index_valid;
    int32_t chunk_count;
    int32_t loaded_chunk_count;
    int32_t center_chunk_x;
    int32_t center_chunk_z;
    int32_t active_render_distance;
    char seed[128];
    terrain_generation_config terrain_config;

    World();
    World(const World &other);
    ~World();
    World &operator=(const World &other);

    int32_t initialize(const char *seed_value);
    void set_terrain_config(const terrain_generation_config &config);
    const terrain_generation_config &terrain_generation_settings() const;
    void destroy();
    int32_t update_around(double camera_x, double camera_z, int32_t generation_budget);
    int32_t update_around(double camera_x, double camera_z, int32_t generation_budget,
                          int32_t render_distance);
    bool validate_visible_distance(double camera_x, double camera_z, double yaw,
                                   int32_t required_distance) const;
    bool surface_top_at(int32_t world_x, int32_t world_z, double *surface_top) const;
    bool solid_block_at(int32_t world_x, int32_t world_y, int32_t world_z) const;
    bool block_id_at(int32_t world_x, int32_t world_y, int32_t world_z, uint32_t *block_id) const;
    int32_t delete_block_at(int32_t world_x, int32_t world_y, int32_t world_z);
    int32_t place_block_at(int32_t world_x, int32_t world_y, int32_t world_z, uint32_t block_id);
    int32_t raycast_solid(double origin_x, double origin_y, double origin_z, double direction_x,
                          double direction_y, double direction_z, double max_distance,
                          int32_t *block_x, int32_t *block_y, int32_t *block_z) const;
    int32_t raycast_edit_target(double origin_x, double origin_y, double origin_z,
                                double direction_x, double direction_y, double direction_z,
                                double max_distance, int32_t *hit_block_x, int32_t *hit_block_y,
                                int32_t *hit_block_z, int32_t *place_block_x,
                                int32_t *place_block_y, int32_t *place_block_z,
                                uint32_t *hit_block_id) const;
    const WorldChunk *find_chunk(int32_t chunk_x, int32_t chunk_z) const;
    WorldChunk *find_chunk_mutable(int32_t chunk_x, int32_t chunk_z);

  private:
    void copy_seed(const char *seed_value);
    void clear_chunk_index();
    void rebuild_chunk_index();
    void register_chunk_index(const WorldChunk &chunk);
    int32_t try_load_chunk_at(int32_t chunk_x, int32_t chunk_z);
    int32_t stream_chunks(int32_t stream_radius, int32_t budget, int32_t *generated);
};

#endif
