#include "../../src/frame/RendererColor.hpp"

RendererColor::RendererColor()
{
}
RendererColor::RendererColor(const RendererColor &other)
{
    (void)other;
}
RendererColor::~RendererColor()
{
}
RendererColor &RendererColor::operator=(const RendererColor &other)
{
    (void)other;
    return *this;
}

uint32_t RendererColor::rgba(uint8_t red, uint8_t green, uint8_t blue)
{
    return (static_cast<uint32_t>(red) << 16U) | (static_cast<uint32_t>(green) << 8U) |
           static_cast<uint32_t>(blue);
}

uint8_t RendererColor::shade_channel(uint8_t channel, double shade)
{
    int32_t value = static_cast<int32_t>(static_cast<double>(channel) * shade);
    if (value < 0)
        value = 0;
    if (value > 255)
        value = 255;
    return static_cast<uint8_t>(value);
}

void RendererColor::base_color_for_block(uint32_t block_id, uint8_t &r, uint8_t &g, uint8_t &b)
{
    struct BlockColor
    {
        uint32_t id;
        uint8_t r, g, b;
    };
    static const BlockColor TABLE[] = {
        {TERRAIN_GENERATOR_GRASS_BLOCK, 91U, 184U, 141U},
        {TERRAIN_GENERATOR_DIRT_BLOCK, 169U, 91U, 96U},
        {TERRAIN_GENERATOR_STONE_BLOCK, 72U, 91U, 122U},
        {TERRAIN_GENERATOR_SHRUB_BLOCK, 190U, 151U, 74U},
        {TERRAIN_GENERATOR_OAK_LOG_BLOCK, 151U, 86U, 98U},
        {TERRAIN_GENERATOR_OAK_LEAVES_BLOCK, 66U, 151U, 132U},
        {TERRAIN_GENERATOR_CACTUS_BLOCK, 67U, 151U, 135U},
        {TERRAIN_GENERATOR_WATER_BLOCK, 30U, 100U, 200U},
    };
    r = 82U;
    g = 72U;
    b = 105U;
    for (size_t i = 0; i < sizeof(TABLE) / sizeof(TABLE[0]); ++i)
    {
        if (TABLE[i].id == block_id)
        {
            r = TABLE[i].r;
            g = TABLE[i].g;
            b = TABLE[i].b;
            return;
        }
    }
}

uint32_t RendererColor::block_color(uint32_t block_id, uint8_t face)
{
    uint8_t r, g, b;
    base_color_for_block(block_id, r, g, b);
    double shade = face_shade(face);
    return rgba(shade_channel(r, shade), shade_channel(g, shade), shade_channel(b, shade));
}

uint32_t RendererColor::shade_color(uint32_t color, double shade)
{
    return rgba(shade_channel(static_cast<uint8_t>((color >> 16U) & 0xFFU), shade),
                shade_channel(static_cast<uint8_t>((color >> 8U) & 0xFFU), shade),
                shade_channel(static_cast<uint8_t>(color & 0xFFU), shade));
}

double RendererColor::face_shade(uint8_t face)
{
    if (face == CHUNK_MESH_FACE_UP)
        return 1.0;
    if (face == CHUNK_MESH_FACE_NORTH || face == CHUNK_MESH_FACE_WEST)
        return 0.58;
    if (face == CHUNK_MESH_FACE_EAST)
        return 0.82;
    return 0.72;
}
