#include "../../src/gpur/GpuTextureAtlas.hpp"

const uint32_t GpuTextureAtlas::BLOCK_IDS[8] = {
    TERRAIN_GENERATOR_GRASS_BLOCK,   TERRAIN_GENERATOR_DIRT_BLOCK,
    TERRAIN_GENERATOR_STONE_BLOCK,   TERRAIN_GENERATOR_SHRUB_BLOCK,
    TERRAIN_GENERATOR_OAK_LOG_BLOCK, TERRAIN_GENERATOR_OAK_LEAVES_BLOCK,
    TERRAIN_GENERATOR_CACTUS_BLOCK,  TERRAIN_GENERATOR_WATER_BLOCK};
const int GpuTextureAtlas::BLOCK_COUNT = 8;

GpuTextureAtlas::GpuTextureAtlas() : _tex(0), _loaded(false)
{
}

GpuTextureAtlas::GpuTextureAtlas(const GpuTextureAtlas &other)
{
    (void)other;
}

GpuTextureAtlas::~GpuTextureAtlas()
{
    destroy();
}

GpuTextureAtlas &GpuTextureAtlas::operator=(const GpuTextureAtlas &other)
{
    (void)other;
    return *this;
}

bool GpuTextureAtlas::upload(const TextureAtlas &atlas)
{
    destroy();
    if (atlas.is_loaded() == false)
        return (false);

    int w = atlas.get_width();
    int h = atlas.get_height();
    if (w <= 0 || h <= 0)
        return (false);

    std::vector<uint8_t> rgba;
    rgba.reserve(static_cast<size_t>(w * h * 4));
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            uint32_t px = atlas.pixel_at(x, y);
            rgba.push_back(static_cast<uint8_t>((px >> 16) & 0xFFU));
            rgba.push_back(static_cast<uint8_t>((px >> 8) & 0xFFU));
            rgba.push_back(static_cast<uint8_t>(px & 0xFFU));
            rgba.push_back(0xFFU);
        }
    }

    glGenTextures(1, &_tex);
    glBindTexture(GL_TEXTURE_2D, _tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    _loaded = true;
    return (true);
}

void GpuTextureAtlas::bind(int unit) const
{
    glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + unit));
    glBindTexture(GL_TEXTURE_2D, _loaded ? _tex : 0);
}

void GpuTextureAtlas::destroy()
{
    if (_tex != 0)
    {
        glDeleteTextures(1, &_tex);
        _tex = 0;
    }
    _loaded = false;
}

void GpuTextureAtlas::fill_tile_uvs(const TextureAtlas &atlas, float uv_out[96 * 4]) const
{

    std::memset(uv_out, 0, 96 * 4 * sizeof(float));
    if (atlas.is_loaded() == false)
        return;

    float atw = static_cast<float>(atlas.get_width());
    float ath = static_cast<float>(atlas.get_height());
    int tw = atlas.get_tile_width();
    int th = atlas.get_tile_height();
    int mx = std::max(2, tw / 32);
    int my = std::max(2, th / 32);

    for (int bi = 0; bi < BLOCK_COUNT; ++bi)
    {
        uint32_t bid = BLOCK_IDS[bi];
        if (bid >= 16U)
            continue;
        for (int face = 0; face < 6; ++face)
        {
            int tx = 2;
            int ty = 2;
            TextureAtlas::atlas_tile_for_block(bid, static_cast<uint8_t>(face), &tx, &ty);
            float u0 = static_cast<float>(tx * tw + mx) / atw;
            float v0 = static_cast<float>(ty * th + my) / ath;
            float us = static_cast<float>(tw - mx * 2) / atw;
            float vs = static_cast<float>(th - my * 2) / ath;
            size_t idx = (static_cast<size_t>(bid) * 6U + static_cast<size_t>(face)) * 4U;
            uv_out[idx + 0] = u0;
            uv_out[idx + 1] = v0;
            uv_out[idx + 2] = us;
            uv_out[idx + 3] = vs;
        }
    }
}

void GpuTextureAtlas::fill_fallback_colors(float colors_out[16 * 3]) const
{
    std::memset(colors_out, 0, 16 * 3 * sizeof(float));
    for (uint32_t bid = 0; bid < 16U; ++bid)
    {
        uint32_t c = RendererColor::block_color(bid, static_cast<uint8_t>(CHUNK_MESH_FACE_UP));
        colors_out[bid * 3 + 0] = static_cast<float>((c >> 16) & 0xFFU) / 255.0f;
        colors_out[bid * 3 + 1] = static_cast<float>((c >> 8) & 0xFFU) / 255.0f;
        colors_out[bid * 3 + 2] = static_cast<float>(c & 0xFFU) / 255.0f;
    }
}

bool GpuTextureAtlas::is_loaded() const
{
    return _loaded;
}
