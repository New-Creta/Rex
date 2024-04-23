#include "rex_directx/resources/vertex_buffer.h"

namespace rex
{
  namespace renderer
  {
    VertexBuffer::VertexBuffer(Device& device, size_t numVertices, size_t vertexStride)
        : Buffer(device, CD3DX12_RESOURCE_DESC::Buffer(numVertices * vertexStride))
        , m_num_vertices(numVertices)
        , m_vertex_stride(vertexStride)
        , m_vertex_buffer_view {}
    {
      create_vertex_buffer_view();
    }

    VertexBuffer::VertexBuffer(Device& device, wrl::ComPtr<ID3D12Resource> resource, size_t numVertices, size_t vertexStride)
        : Buffer(device, resource)
        , m_num_vertices(numVertices)
        , m_vertex_stride(vertexStride)
        , m_vertex_buffer_view {}
    {
      create_vertex_buffer_view();
    }

    VertexBuffer::~VertexBuffer() = default;

    size_t VertexBuffer::num_vertices() const
    {
      return m_num_vertices;
    }

    size_t VertexBuffer::vertex_stride() const
    {
      return m_vertex_stride;
    }

    D3D12_VERTEX_BUFFER_VIEW VertexBuffer::vertex_buffer_view() const
    {
      return m_vertex_buffer_view;
    }

    void VertexBuffer::create_vertex_buffer_view()
    {
      m_vertex_buffer_view.BufferLocation = d3d_resource()->GetGPUVirtualAddress();
      m_vertex_buffer_view.SizeInBytes    = static_cast<UINT>(m_num_vertices * m_vertex_stride);
      m_vertex_buffer_view.StrideInBytes  = static_cast<UINT>(m_vertex_stride);
    }
  } // namespace renderer
} // namespace rex