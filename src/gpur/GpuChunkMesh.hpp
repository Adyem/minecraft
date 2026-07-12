#ifndef GPUR_CHUNK_MESH_HPP
#define GPUR_CHUNK_MESH_HPP

#include "../ft_vox.hpp"

class GpuChunkMesh
{
    GLuint _vao;
    GLuint _vbo;
    GLuint _ebo;
    size_t _uploaded_vertices;
    size_t _uploaded_indices;
    int32_t _index_count;

  public:
    GpuChunkMesh();
    GpuChunkMesh(const GpuChunkMesh &other);
    ~GpuChunkMesh();
    GpuChunkMesh &operator=(const GpuChunkMesh &other);

    void sync(const chunk_mesh &mesh);
    void draw() const;
    void destroy();
    bool has_geometry() const;

  private:
    void alloc_buffers();
    void upload_geometry(const chunk_mesh &mesh, size_t vcount, size_t icount);
    void setup_attributes();
};

#endif
