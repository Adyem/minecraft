#include "../../src/gpur/GpuChunkMesh.hpp"

GpuChunkMesh::GpuChunkMesh()
    : _vao(0), _vbo(0), _ebo(0), _uploaded_vertices(0), _uploaded_indices(0), _index_count(0)
{
}

GpuChunkMesh::GpuChunkMesh(const GpuChunkMesh &other)
{
    (void)other;
}

GpuChunkMesh::~GpuChunkMesh()
{
    destroy();
}

GpuChunkMesh &GpuChunkMesh::operator=(const GpuChunkMesh &other)
{
    (void)other;
    return *this;
}

void GpuChunkMesh::alloc_buffers()
{
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_ebo);
}

void GpuChunkMesh::upload_geometry(const chunk_mesh &mesh, size_t vcount, size_t icount)
{
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vcount * sizeof(chunk_mesh_vertex)),
                 vcount > 0 ? static_cast<const void *>(mesh.vertices.begin()) : nullptr,
                 GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(icount * sizeof(uint32_t)),
                 icount > 0 ? static_cast<const void *>(mesh.indices.begin()) : nullptr,
                 GL_DYNAMIC_DRAW);
}

void GpuChunkMesh::setup_attributes()
{
    constexpr GLsizei stride = static_cast<GLsizei>(sizeof(chunk_mesh_vertex));
    glVertexAttribIPointer(0, 3, GL_UNSIGNED_SHORT, stride, nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribIPointer(1, 2, GL_UNSIGNED_SHORT, stride,
                           reinterpret_cast<const void *>(static_cast<uintptr_t>(6)));
    glEnableVertexAttribArray(1);
    glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, stride,
                           reinterpret_cast<const void *>(static_cast<uintptr_t>(12)));
    glEnableVertexAttribArray(2);
    glVertexAttribIPointer(3, 1, GL_UNSIGNED_BYTE, stride,
                           reinterpret_cast<const void *>(static_cast<uintptr_t>(16)));
    glEnableVertexAttribArray(3);
}

void GpuChunkMesh::sync(const chunk_mesh &mesh)
{
    size_t vcount = mesh.vertices.size();
    size_t icount = mesh.indices.size();
    if (vcount == _uploaded_vertices && icount == _uploaded_indices)
        return;
    if (_vao == 0)
        alloc_buffers();
    glBindVertexArray(_vao);
    upload_geometry(mesh, vcount, icount);
    setup_attributes();
    glBindVertexArray(0);
    _uploaded_vertices = vcount;
    _uploaded_indices = icount;
    _index_count = static_cast<int32_t>(icount);
}

void GpuChunkMesh::draw() const
{
    if (_vao == 0 || _index_count <= 0)
        return;
    glBindVertexArray(_vao);
    glDrawElements(GL_TRIANGLES, _index_count, GL_UNSIGNED_INT, nullptr);
}

void GpuChunkMesh::destroy()
{
    if (_vao != 0)
    {
        glDeleteVertexArrays(1, &_vao);
        glDeleteBuffers(1, &_vbo);
        glDeleteBuffers(1, &_ebo);
        _vao = 0;
        _vbo = 0;
        _ebo = 0;
    }
    _uploaded_vertices = 0;
    _uploaded_indices = 0;
    _index_count = 0;
}

bool GpuChunkMesh::has_geometry() const
{
    return _index_count > 0;
}
