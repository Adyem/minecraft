#include "../../src/chunks/WorldChunkLoader.hpp"

WorldChunkLoader::WorldChunkLoader()
{
}

WorldChunkLoader::WorldChunkLoader(const WorldChunkLoader &other)
{
    (void)other;
}

WorldChunkLoader::~WorldChunkLoader()
{
}

WorldChunkLoader &WorldChunkLoader::operator=(const WorldChunkLoader &other)
{
    (void)other;
    return *this;
}

int32_t WorldChunkLoader::lookup_block(void *user_data, int32_t world_x, int32_t world_y,
                                       int32_t world_z, uint32_t *block_id)
{
    if (!user_data || !block_id)
        return FT_ERR_INVALID_ARGUMENT;
    NeighborContext *ctx = static_cast<NeighborContext *>(user_data);
    if (!ctx->chunks || ctx->chunk_count <= 0 || world_y < 0 || world_y >= GAME_VOXEL_CHUNK_HEIGHT)
    {
        *block_id = GAME_VOXEL_AIR_BLOCK;
        return FT_ERR_SUCCESS;
    }
    int32_t cx = WorldCoordinates::floor_divide(world_x, GAME_VOXEL_CHUNK_WIDTH);
    int32_t cz = WorldCoordinates::floor_divide(world_z, GAME_VOXEL_CHUNK_DEPTH);
    const WorldChunk *wc = WorldChunkStore::find_chunk(ctx->chunks, ctx->chunk_count, cx, cz);
    if (!wc || !wc->initialized)
    {
        *block_id = GAME_VOXEL_AIR_BLOCK;
        return FT_ERR_SUCCESS;
    }
    return wc->chunk.read_block(world_x - cx * GAME_VOXEL_CHUNK_WIDTH, world_y,
                                world_z - cz * GAME_VOXEL_CHUNK_DEPTH, block_id);
}

int32_t WorldChunkLoader::setup_chunk_coordinates(WorldChunk *world_chunk, int32_t chunk_x,
                                                  int32_t chunk_z)
{
    if (!world_chunk)
        return FT_ERR_INVALID_ARGUMENT;
    world_chunk->chunk_x = chunk_x;
    world_chunk->chunk_z = chunk_z;
    world_chunk->world_x = chunk_x * GAME_VOXEL_CHUNK_WIDTH;
    world_chunk->world_z = chunk_z * GAME_VOXEL_CHUNK_DEPTH;
    world_chunk->initialized = false;
    return FT_ERR_SUCCESS;
}

int32_t WorldChunkLoader::init_chunk_data(WorldChunk *world_chunk, const char *seed)
{
    int32_t err = world_chunk->chunk.initialize();
    if (err != FT_ERR_SUCCESS)
        return err;
    err = chunk_mesh_initialize(world_chunk->mesh);
    if (err != FT_ERR_SUCCESS)
    {
        (void)world_chunk->chunk.destroy();
        return err;
    }
    err = terrain_generate_chunk(world_chunk->chunk, world_chunk->world_x, world_chunk->world_z,
                                 seed);
    if (err != FT_ERR_SUCCESS)
    {
        (void)chunk_mesh_destroy(world_chunk->mesh);
        (void)world_chunk->chunk.destroy();
    }
    return err;
}

int32_t WorldChunkLoader::build_mesh_with_neighbors(WorldChunk *world_chunk, int32_t chunk_x,
                                                    int32_t chunk_z, WorldChunk *chunks,
                                                    int32_t chunk_count)
{
    NeighborContext ctx = {chunks, chunk_count};
    int32_t err = chunk_mesh_generate_from_chunk_with_neighbors(
        world_chunk->mesh, world_chunk->chunk, chunk_x, chunk_z, &lookup_block, &ctx);
    if (err != FT_ERR_SUCCESS)
    {
        (void)chunk_mesh_destroy(world_chunk->mesh);
        (void)world_chunk->chunk.destroy();
    }
    return err;
}

int32_t WorldChunkLoader::initialize_chunk(WorldChunk *world_chunk, int32_t chunk_x,
                                           int32_t chunk_z, const char *seed)
{
    int32_t err = setup_chunk_coordinates(world_chunk, chunk_x, chunk_z);
    if (err != FT_ERR_SUCCESS)
        return err;
    err = init_chunk_data(world_chunk, seed);
    if (err != FT_ERR_SUCCESS)
        return err;
    err = chunk_mesh_generate_from_chunk(world_chunk->mesh, world_chunk->chunk);
    if (err != FT_ERR_SUCCESS)
    {
        (void)chunk_mesh_destroy(world_chunk->mesh);
        (void)world_chunk->chunk.destroy();
        return err;
    }
    world_chunk->initialized = true;
    return FT_ERR_SUCCESS;
}

int32_t WorldChunkLoader::initialize_chunk(WorldChunk *world_chunk, int32_t chunk_x,
                                           int32_t chunk_z, const char *seed, WorldChunk *chunks,
                                           int32_t chunk_count)
{
    int32_t err = setup_chunk_coordinates(world_chunk, chunk_x, chunk_z);
    if (err != FT_ERR_SUCCESS)
        return err;
    err = init_chunk_data(world_chunk, seed);
    if (err != FT_ERR_SUCCESS)
        return err;
    err = build_mesh_with_neighbors(world_chunk, chunk_x, chunk_z, chunks, chunk_count);
    if (err != FT_ERR_SUCCESS)
        return err;
    world_chunk->initialized = true;
    return FT_ERR_SUCCESS;
}

int32_t WorldChunkLoader::generate_missing_chunk(WorldChunk *chunks, int32_t chunk_count,
                                                 int32_t *loaded_chunk_count, int32_t chunk_x,
                                                 int32_t chunk_z, const char *seed)
{
    if (WorldChunkStore::find_chunk(chunks, chunk_count, chunk_x, chunk_z))
        return FT_ERR_SUCCESS;
    WorldChunk *slot = WorldChunkStore::find_free_chunk_slot(chunks, chunk_count);
    if (!slot)
        return FT_ERR_NO_MEMORY;
    int32_t err = initialize_chunk(slot, chunk_x, chunk_z, seed);
    if (err == FT_ERR_SUCCESS && loaded_chunk_count)
        *loaded_chunk_count += 1;
    return err;
}

int32_t WorldChunkLoader::remesh_chunk(WorldChunk *chunks, int32_t chunk_count, int32_t chunk_x,
                                       int32_t chunk_z)
{
    WorldChunk *wc = WorldChunkStore::find_chunk_mutable(chunks, chunk_count, chunk_x, chunk_z);
    if (!wc)
        return FT_ERR_SUCCESS;
    int32_t err = chunk_mesh_clear(wc->mesh);
    if (err == FT_ERR_SUCCESS)
        err = chunk_mesh_generate_from_chunk(wc->mesh, wc->chunk);
    return err;
}

int32_t WorldChunkLoader::remesh_chunk(WorldChunk *chunks, int32_t chunk_count, int32_t chunk_x,
                                       int32_t chunk_z, bool use_neighbors)
{
    WorldChunk *wc = WorldChunkStore::find_chunk_mutable(chunks, chunk_count, chunk_x, chunk_z);
    if (!wc)
        return FT_ERR_SUCCESS;
    int32_t err = chunk_mesh_clear(wc->mesh);
    if (err != FT_ERR_SUCCESS)
        return err;
    if (!use_neighbors)
        return chunk_mesh_generate_from_chunk(wc->mesh, wc->chunk);
    NeighborContext ctx = {chunks, chunk_count};
    return chunk_mesh_generate_from_chunk_with_neighbors(wc->mesh, wc->chunk, chunk_x, chunk_z,
                                                         &lookup_block, &ctx);
}

int32_t WorldChunkLoader::remesh_edited_chunk_border(WorldChunk *chunks, int32_t chunk_count,
                                                     int32_t chunk_x, int32_t chunk_z,
                                                     int32_t local_x, int32_t local_z)
{
    int32_t err = remesh_chunk(chunks, chunk_count, chunk_x, chunk_z, true);
    if (err != FT_ERR_SUCCESS)
        return err;
    if (local_x == 0)
        err = remesh_chunk(chunks, chunk_count, chunk_x - 1, chunk_z, true);
    if (err == FT_ERR_SUCCESS && local_x == GAME_VOXEL_CHUNK_WIDTH - 1)
        err = remesh_chunk(chunks, chunk_count, chunk_x + 1, chunk_z, true);
    if (err == FT_ERR_SUCCESS && local_z == 0)
        err = remesh_chunk(chunks, chunk_count, chunk_x, chunk_z - 1, true);
    if (err == FT_ERR_SUCCESS && local_z == GAME_VOXEL_CHUNK_DEPTH - 1)
        err = remesh_chunk(chunks, chunk_count, chunk_x, chunk_z + 1, true);
    return err;
}
