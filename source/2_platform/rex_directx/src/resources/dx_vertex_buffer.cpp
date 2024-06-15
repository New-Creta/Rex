#include "rex_directx/resources/dx_vertex_buffer.h"

namespace rex
{
  namespace gfx
  {
    DxVertexBuffer::DxVertexBuffer(const wrl::ComPtr<ID3D12Resource>& resource, s32 numVertices, rsl::memory_size vertexSize)
      : VertexBuffer(numVertices, vertexSize)
      , m_resource(resource)
    {
    }

    ID3D12Resource* DxVertexBuffer::dx_object()
    {
      return m_resource.Get();
    }

  }
}