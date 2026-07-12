#ifndef RENDER_DEBUG_HPP
#define RENDER_DEBUG_HPP


#include "../ft_vox.hpp"
class RenderDebug
{
  public:
    double fps;
    double frame_ms;
    int32_t visible_chunks;
    int32_t loaded_chunks;
    int32_t render_distance;
    uint32_t selected_block_id;
    uint32_t ram_mb;
    uint32_t vram_approx_mb;
    double camera_x;
    double camera_y;
    double camera_z;
    double camera_speed;
    double boost_speed;
    char seed[32];
    char biome_name[24];

    RenderDebug();
    RenderDebug(const RenderDebug &other);
    ~RenderDebug();
    RenderDebug &operator=(const RenderDebug &other);

    void clear();
};

#endif
