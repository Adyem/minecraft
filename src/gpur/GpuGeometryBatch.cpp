#include "../../src/gpur/GpuGeometryBatch.hpp"

GpuGeometryBatch::GpuGeometryBatch()
    : _mega_vao(0), _mega_vbo(0), _mega_ebo(0), _mega_vbo_cap(0), _mega_ebo_cap(0), _water_vao(0),
      _water_ebo(0), _water_ebo_cap(0), _owns_vbo(true)
{
}

GpuGeometryBatch::GpuGeometryBatch(const GpuGeometryBatch &other)
    : _mega_vao(0), _mega_vbo(0), _mega_ebo(0), _mega_vbo_cap(0), _mega_ebo_cap(0), _water_vao(0),
      _water_ebo(0), _water_ebo_cap(0), _owns_vbo(true)
{
    (void)other;
}

GpuGeometryBatch::~GpuGeometryBatch()
{
    destroy();
}

GpuGeometryBatch &GpuGeometryBatch::operator=(const GpuGeometryBatch &other)
{
    (void)other;
    return *this;
}

void GpuGeometryBatch::bind_vertex_attribs()
{
    constexpr GLsizei S = static_cast<GLsizei>(sizeof(Vertex));
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, S, nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribIPointer(1, 2, GL_UNSIGNED_SHORT, S, reinterpret_cast<const void *>(static_cast<uintptr_t>(12)));
    glEnableVertexAttribArray(1);
    glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, S, reinterpret_cast<const void *>(static_cast<uintptr_t>(16)));
    glEnableVertexAttribArray(2);
    glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, S, reinterpret_cast<const void *>(static_cast<uintptr_t>(20)));
    glEnableVertexAttribArray(3);
}

void GpuGeometryBatch::setup_world_vao()
{
    glGenVertexArrays(1, &_mega_vao);
    glGenBuffers(1, &_mega_vbo);
    glGenBuffers(1, &_mega_ebo);
    glBindVertexArray(_mega_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _mega_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _mega_ebo);
    bind_vertex_attribs();
    glBindVertexArray(0);
}

void GpuGeometryBatch::setup_water_vao()
{
    glGenVertexArrays(1, &_water_vao);
    glGenBuffers(1, &_water_ebo);
    glBindVertexArray(_water_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _mega_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _water_ebo);
    bind_vertex_attribs();
    glBindVertexArray(0);
}

bool GpuGeometryBatch::initialize(GLuint shared_vbo)
{
    destroy();
    if (shared_vbo != 0)
    {
        _mega_vbo = shared_vbo;
        _owns_vbo = false;
    }
    setup_world_vao();
    setup_water_vao();
    return true;
}

void GpuGeometryBatch::destroy()
{
    if (_mega_vao != 0)
    {
        glDeleteVertexArrays(1, &_mega_vao);
        _mega_vao = 0;
    }
    if (_owns_vbo && _mega_vbo != 0)
    {
        glDeleteBuffers(1, &_mega_vbo);
        _mega_vbo = 0;
    }
    if (_mega_ebo != 0)
    {
        glDeleteBuffers(1, &_mega_ebo);
        _mega_ebo = 0;
    }
    _mega_vbo_cap = 0;
    _mega_ebo_cap = 0;
    _mega_verts.clear();
    _mega_idxs.clear();
    if (_water_vao != 0)
    {
        glDeleteVertexArrays(1, &_water_vao);
        _water_vao = 0;
    }
    if (_water_ebo != 0)
    {
        glDeleteBuffers(1, &_water_ebo);
        _water_ebo = 0;
    }
    _water_ebo_cap = 0;
    _water_idxs.clear();
}

void GpuGeometryBatch::add_chunk(const WorldChunk &wc, uint32_t base)
{
    const float ox = static_cast<float>(wc.world_x);
    const float oz = static_cast<float>(wc.world_z);
    for (const chunk_mesh_vertex &v : wc.mesh.vertices)
    {
        Vertex mv;
        mv.x = static_cast<float>(v.coordinate_x) + ox;
        mv.y = static_cast<float>(v.coordinate_y);
        mv.z = static_cast<float>(v.coordinate_z) + oz;
        mv.tu = v.texture_u;
        mv.tv = v.texture_v;
        mv.block_id = v.block_id;
        mv.face = static_cast<uint32_t>(v.face);
        _mega_verts.push_back(mv);
    }
    for (size_t j = 0; j < wc.mesh.indices.size(); ++j)
    {
        const uint32_t raw = wc.mesh.indices[j];
        if (wc.mesh.vertices[raw].block_id == TERRAIN_GENERATOR_WATER_BLOCK)
            _water_idxs.push_back(base + raw);
        else
            _mega_idxs.push_back(base + raw);
    }
}

void GpuGeometryBatch::collect(const Camera &camera, const World &world, int width, int height)
{
    _mega_verts.clear();
    _mega_idxs.clear();
    _water_idxs.clear();
    RenderCache cache;
    cache.configure(camera, width, height, world.active_render_distance);
    for (int i = 0; i < world.chunk_count; ++i)
    {
        const WorldChunk &wc = world.chunks[i];
        if (!wc.initialized || wc.mesh.vertices.empty() == FT_TRUE)
            continue;
        if (!MeshCuller::chunk_is_visible(camera, wc, cache))
            continue;
        add_chunk(wc, static_cast<uint32_t>(_mega_verts.size()));
    }
}

void GpuGeometryBatch::upload_solid_buffers()
{
    const GLsizeiptr vb = static_cast<GLsizeiptr>(_mega_verts.size() * sizeof(Vertex));
    const GLsizeiptr ib = static_cast<GLsizeiptr>(_mega_idxs.size() * sizeof(uint32_t));
    glBindBuffer(GL_ARRAY_BUFFER, _mega_vbo);
    if (static_cast<size_t>(vb) > _mega_vbo_cap)
    {
        glBufferData(GL_ARRAY_BUFFER, vb, nullptr, GL_DYNAMIC_DRAW);
        _mega_vbo_cap = static_cast<size_t>(vb);
    }
    glBufferSubData(GL_ARRAY_BUFFER, 0, vb, _mega_verts.data());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _mega_ebo);
    if (static_cast<size_t>(ib) > _mega_ebo_cap)
    {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, ib, nullptr, GL_DYNAMIC_DRAW);
        _mega_ebo_cap = static_cast<size_t>(ib);
    }
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, ib, _mega_idxs.data());
}

void GpuGeometryBatch::flush_solid(GLuint, GLint u_mvp, const float mvp[16], GpuTextureAtlas &atlas)
{
    if (_mega_verts.empty())
        return;
    upload_solid_buffers();
    glUniformMatrix4fv(u_mvp, 1, GL_FALSE, mvp);
    atlas.bind(0);
    glBindVertexArray(_mega_vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_mega_idxs.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void GpuGeometryBatch::upload_water_buffer()
{
    const GLsizeiptr ib = static_cast<GLsizeiptr>(_water_idxs.size() * sizeof(uint32_t));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _water_ebo);
    if (static_cast<size_t>(ib) > _water_ebo_cap)
    {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, ib, nullptr, GL_DYNAMIC_DRAW);
        _water_ebo_cap = static_cast<size_t>(ib);
    }
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, ib, _water_idxs.data());
}

void GpuGeometryBatch::flush_water(GLuint, GLint u_mvp, const float mvp[16], GpuTextureAtlas &atlas)
{
    if (_water_idxs.empty())
        return;
    upload_water_buffer();
    glUniformMatrix4fv(u_mvp, 1, GL_FALSE, mvp);
    atlas.bind(0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    glBindVertexArray(_water_vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_water_idxs.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

size_t GpuGeometryBatch::gpu_bytes() const
{
    return _mega_vbo_cap + _mega_ebo_cap + _water_ebo_cap;
}
