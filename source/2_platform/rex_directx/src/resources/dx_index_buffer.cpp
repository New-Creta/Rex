#include "rex_directx/resources/dx_index_buffer.h"

namespace rex
{
  namespace gfx
  {
    DxIndexBuffer::DxIndexBuffer(const wrl::ComPtr<ID3D12Resource>& resource, s32 numIndices, IndexBufferFormat format)
      : IndexBuffer(numIndices, format)
      , m_resource(resource)
    {}

    ID3D12Resource* DxIndexBuffer::dx_object()
    {
      return m_resource.Get();
    }

  }
}