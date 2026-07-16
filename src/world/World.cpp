#include "../../src/world/World.hpp"

World::World(const World &other) : stream_candidates_radius_(-1) { (void)other; }
World &World::operator=(const World &other)
{ (void)other; return (*this); }


World::World()
{
    this->chunk_count = WorldCoordinates::CHUNK_COUNT;
    this->loaded_chunk_count = 0;
    this->center_chunk_x = 0;
    this->center_chunk_z = 0;
    this->chunk_index_center_x = 0;
    this->chunk_index_center_z = 0;
    this->active_render_distance = WorldCoordinates::REQUIRED_VISIBLE_DISTANCE;
    this->stream_candidates_radius_ = -1;
    this->seed[0] = '\0';
    this->terrain_config = terrain_default_generation_config();
    this->clear_chunk_index();
}

World::~World()
{
    this->destroy();
}

void World::copy_seed(const char *seed_value)
{
    if (seed_value == nullptr)
        seed_value = "";
    std::strncpy(this->seed, seed_value, sizeof(this->seed) - 1U);
    this->seed[sizeof(this->seed) - 1U] = '\0';
}

void World::clear_chunk_index()
{
    int32_t index;

    index = 0;
    while (index < WorldCoordinates::CHUNK_COUNT)
    {
        this->chunk_index[index] = nullptr;
        index = index + 1;
    }
    this->chunk_index_valid = false;
}

void World::rebuild_chunk_index()
{
    int32_t index;
    int32_t slot_x;
    int32_t slot_z;
    int32_t grid_x;
    int32_t grid_z;
    int32_t grid_width;

    this->clear_chunk_index();
    grid_width = WorldCoordinates::CACHE_CHUNK_RADIUS * 2 + 1;
    index = 0;
    while (index < this->chunk_count)
    {
        if (this->chunks[index].initialized == true)
        {
            slot_x = this->chunks[index].chunk_x - this->center_chunk_x;
            slot_z = this->chunks[index].chunk_z - this->center_chunk_z;
            if (slot_x >= -WorldCoordinates::CACHE_CHUNK_RADIUS &&
                slot_x <= WorldCoordinates::CACHE_CHUNK_RADIUS &&
                slot_z >= -WorldCoordinates::CACHE_CHUNK_RADIUS &&
                slot_z <= WorldCoordinates::CACHE_CHUNK_RADIUS)
            {
                grid_x = slot_x + WorldCoordinates::CACHE_CHUNK_RADIUS;
                grid_z = slot_z + WorldCoordinates::CACHE_CHUNK_RADIUS;
                this->chunk_index[(grid_z * grid_width) + grid_x] = &this->chunks[index];
            }
        }
        index = index + 1;
    }
    this->chunk_index_center_x = this->center_chunk_x;
    this->chunk_index_center_z = this->center_chunk_z;
    this->chunk_index_valid = true;
}

void World::register_chunk_index(const WorldChunk &chunk)
{
    int32_t slot_x;
    int32_t slot_z;
    int32_t grid_width;

    if (this->chunk_index_valid == false)
        return;
    slot_x = chunk.chunk_x - this->chunk_index_center_x;
    slot_z = chunk.chunk_z - this->chunk_index_center_z;
    if (slot_x < -WorldCoordinates::CACHE_CHUNK_RADIUS ||
        slot_x > WorldCoordinates::CACHE_CHUNK_RADIUS ||
        slot_z < -WorldCoordinates::CACHE_CHUNK_RADIUS ||
        slot_z > WorldCoordinates::CACHE_CHUNK_RADIUS)
        return;
    grid_width = WorldCoordinates::CACHE_CHUNK_RADIUS * 2 + 1;
    this->chunk_index[((slot_z + WorldCoordinates::CACHE_CHUNK_RADIUS) * grid_width) +
                      (slot_x + WorldCoordinates::CACHE_CHUNK_RADIUS)] =
        const_cast<WorldChunk *>(&chunk);
}

static void world_remesh_loaded_neighbor(World *world, int32_t chunk_x, int32_t chunk_z)
{
    if (world == nullptr)
        return;
    if (world->find_chunk(chunk_x, chunk_z) == nullptr)
        return;
    (void)WorldChunkLoader::remesh_chunk(world->chunks, world->chunk_count, chunk_x, chunk_z, true);
}

int32_t World::initialize(const char *seed_value)
{
    int32_t index;
    int32_t error_code;

    this->destroy();
    this->chunk_count = WorldCoordinates::CHUNK_COUNT;
    this->loaded_chunk_count = 0;
    this->center_chunk_x = 0;
    this->center_chunk_z = 0;
    this->active_render_distance = WorldCoordinates::REQUIRED_VISIBLE_DISTANCE;
    this->clear_chunk_index();
    this->copy_seed(seed_value);
    index = 0;
    while (index < this->chunk_count)
    {
        this->chunks[index].reset_coordinates();
        this->chunks[index].initialized = false;
        index = index + 1;
    }
    // Keep first launch bounded. The game loop keeps streaming chunks after init,
    // so we only need a small starter set here instead of filling the whole ring.
    error_code =
        this->update_around(0.0, 0.0, 64, this->active_render_distance);
    if (error_code != FT_ERR_SUCCESS)
        this->destroy();
    return (error_code);
}

void World::prepare_stream_candidates(int32_t stream_radius)
{
    if (this->stream_candidates_radius_ == stream_radius)
        return;
    this->stream_candidates_.clear();
    this->stream_candidates_.reserve(static_cast<size_t>((stream_radius * 2 + 1) *
                                                         (stream_radius * 2 + 1)));
    const int32_t radius_sq = stream_radius * stream_radius;
    for (int32_t z = -stream_radius; z <= stream_radius; ++z)
    {
        for (int32_t x = -stream_radius; x <= stream_radius; ++x)
        {
            const int32_t dist_sq = (x * x) + (z * z);
            if (dist_sq <= radius_sq)
                this->stream_candidates_.push_back({x, z, dist_sq});
        }
    }
    std::sort(this->stream_candidates_.begin(), this->stream_candidates_.end(),
              [](const StreamCandidate &a, const StreamCandidate &b) -> bool
              {
                  if (a.dist_sq != b.dist_sq)
                      return a.dist_sq < b.dist_sq;
                  const int32_t a_manhattan = std::abs(a.offset_x) + std::abs(a.offset_z);
                  const int32_t b_manhattan = std::abs(b.offset_x) + std::abs(b.offset_z);
                  if (a_manhattan != b_manhattan)
                      return a_manhattan < b_manhattan;
                  if (a.offset_z != b.offset_z)
                      return a.offset_z < b.offset_z;
                  return a.offset_x < b.offset_x;
              });
    this->stream_candidates_radius_ = stream_radius;
}

void World::set_terrain_config(const terrain_generation_config &config)
{
    this->terrain_config = config;
}

const terrain_generation_config &World::terrain_generation_settings() const
{
    return (this->terrain_config);
}

void World::destroy()
{
    int32_t index;

    index = 0;
    while (index < this->chunk_count)
    {
        this->chunks[index].destroy();
        index = index + 1;
    }
    this->loaded_chunk_count = 0;
    this->clear_chunk_index();
}

int32_t World::update_around(double camera_x, double camera_z, int32_t generation_budget)
{
    return (this->update_around(camera_x, camera_z, generation_budget,
                                WorldCoordinates::REQUIRED_VISIBLE_DISTANCE));
}

int32_t World::try_load_chunk_at(int32_t chunk_x, int32_t chunk_z)
{
    WorldChunk *slot;
    int32_t error_code;

    if (this->find_chunk(chunk_x, chunk_z) != nullptr)
        return (0);
    slot = WorldChunkStore::find_free_chunk_slot(this->chunks, this->chunk_count);
    if (slot == nullptr)
        return (FT_ERR_NO_MEMORY);
    error_code = WorldChunkLoader::initialize_chunk(slot, chunk_x, chunk_z, this->seed,
                                                    this->chunks, this->chunk_count,
                                                    this->terrain_config);
    if (error_code != FT_ERR_SUCCESS)
        return (error_code);
    this->loaded_chunk_count = this->loaded_chunk_count + 1;
    this->register_chunk_index(*slot);
    world_remesh_loaded_neighbor(this, chunk_x - 1, chunk_z);
    world_remesh_loaded_neighbor(this, chunk_x + 1, chunk_z);
    world_remesh_loaded_neighbor(this, chunk_x, chunk_z - 1);
    world_remesh_loaded_neighbor(this, chunk_x, chunk_z + 1);
    return (1);
}

int32_t World::stream_chunks(int32_t stream_radius, int32_t budget, int32_t *generated)
{
    int32_t result;
    this->prepare_stream_candidates(stream_radius);
    for (const StreamCandidate &candidate : this->stream_candidates_)
    {
        result = this->try_load_chunk_at(this->center_chunk_x + candidate.offset_x,
                                         this->center_chunk_z + candidate.offset_z);
        if (result < 0)
            return (result);
        if (result > 0)
        {
            *generated = *generated + 1;
            if (budget > 0 && *generated >= budget)
                return (FT_ERR_SUCCESS);
        }
    }
    return (FT_ERR_SUCCESS);
}

int32_t World::update_around(double camera_x, double camera_z, int32_t generation_budget,
                             int32_t render_distance)
{
    int32_t stream_radius;
    int32_t generated;

    this->center_chunk_x = WorldCoordinates::floor_divide(
        static_cast<int32_t>(std::floor(camera_x)), GAME_VOXEL_CHUNK_WIDTH);
    this->center_chunk_z = WorldCoordinates::floor_divide(
        static_cast<int32_t>(std::floor(camera_z)), GAME_VOXEL_CHUNK_DEPTH);
    this->active_render_distance =
        WorldCoordinates::clamp_int(render_distance, WorldCoordinates::MIN_RENDER_DISTANCE,
                                    WorldCoordinates::CACHE_CHUNK_RADIUS * GAME_VOXEL_CHUNK_WIDTH);
    if (this->chunk_index_valid == false || this->chunk_index_center_x != this->center_chunk_x ||
        this->chunk_index_center_z != this->center_chunk_z)
    {
        WorldChunkStore::evict_far_chunks(this->chunks, this->chunk_count,
                                          &this->loaded_chunk_count, this->center_chunk_x,
                                          this->center_chunk_z);
        this->rebuild_chunk_index();
    }
    stream_radius = WorldCoordinates::render_distance_to_chunk_radius(this->active_render_distance);
    if (generation_budget <= 0)
        return (FT_ERR_SUCCESS);
    generated = 0;
    return (this->stream_chunks(stream_radius, generation_budget, &generated));
}

const WorldChunk *World::find_chunk(int32_t chunk_x, int32_t chunk_z) const
{
    int32_t slot_x;
    int32_t slot_z;
    int32_t grid_width;

    slot_x = chunk_x - this->center_chunk_x;
    slot_z = chunk_z - this->center_chunk_z;
    if (slot_x < -WorldCoordinates::CACHE_CHUNK_RADIUS ||
        slot_x > WorldCoordinates::CACHE_CHUNK_RADIUS ||
        slot_z < -WorldCoordinates::CACHE_CHUNK_RADIUS ||
        slot_z > WorldCoordinates::CACHE_CHUNK_RADIUS)
        return (nullptr);
    grid_width = WorldCoordinates::CACHE_CHUNK_RADIUS * 2 + 1;
    return (this->chunk_index[((slot_z + WorldCoordinates::CACHE_CHUNK_RADIUS) * grid_width) +
                              (slot_x + WorldCoordinates::CACHE_CHUNK_RADIUS)]);
}

WorldChunk *World::find_chunk_mutable(int32_t chunk_x, int32_t chunk_z)
{
    return (
        const_cast<WorldChunk *>(static_cast<const World *>(this)->find_chunk(chunk_x, chunk_z)));
}

bool World::validate_visible_distance(double camera_x, double camera_z, double yaw,
                                      int32_t required_distance) const
{
    return (WorldVisibilityValidator::validate_visible_distance(*this, camera_x, camera_z, yaw,
                                                                required_distance));
}

bool World::surface_top_at(int32_t world_x, int32_t world_z, double *surface_top) const
{
    return (WorldBlockQuery::surface_top_at(*this, world_x, world_z, surface_top));
}

bool World::solid_block_at(int32_t world_x, int32_t world_y, int32_t world_z) const
{
    return (WorldBlockQuery::solid_block_at(*this, world_x, world_y, world_z));
}

bool World::block_id_at(int32_t world_x, int32_t world_y, int32_t world_z, uint32_t *block_id) const
{
    return (WorldBlockQuery::block_id_at(*this, world_x, world_y, world_z, block_id));
}

int32_t World::delete_block_at(int32_t world_x, int32_t world_y, int32_t world_z)
{
    return (WorldBlockEditor::delete_block_at(*this, world_x, world_y, world_z));
}

int32_t World::place_block_at(int32_t world_x, int32_t world_y, int32_t world_z, uint32_t block_id)
{
    return (WorldBlockEditor::place_block_at(*this, world_x, world_y, world_z, block_id));
}

int32_t World::raycast_solid(double origin_x, double origin_y, double origin_z, double direction_x,
                             double direction_y, double direction_z, double max_distance,
                             int32_t *block_x, int32_t *block_y, int32_t *block_z) const
{
    return (WorldRaycaster::raycast_solid(*this, origin_x, origin_y, origin_z, direction_x,
                                          direction_y, direction_z, max_distance, block_x, block_y,
                                          block_z));
}

int32_t World::raycast_edit_target(double origin_x, double origin_y, double origin_z,
                                   double direction_x, double direction_y, double direction_z,
                                   double max_distance, int32_t *hit_block_x, int32_t *hit_block_y,
                                   int32_t *hit_block_z, int32_t *place_block_x,
                                   int32_t *place_block_y, int32_t *place_block_z,
                                   uint32_t *hit_block_id) const
{
    return (WorldRaycaster::raycast_edit_target(*this, origin_x, origin_y, origin_z, direction_x,
                                                direction_y, direction_z, max_distance, hit_block_x,
                                                hit_block_y, hit_block_z, place_block_x,
                                                place_block_y, place_block_z, hit_block_id));
}
