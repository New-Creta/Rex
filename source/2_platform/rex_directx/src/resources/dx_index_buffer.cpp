#include "rex_directx/resources/dx_index_buffer.h"

namespace rex
{
  namespace gfx
  {
    DxIndexBuffer::DxIndexBuffer(const wrl::ComPtr<ID3D12Resource>& resource, s32 numIndices, IndexBufferFormat format)
      : IndexBuffer(numIndices, format)
      , DxResource(resource)
    {}

    void* DxIndexBuffer::api_object() const
    {
      return dx_object();
    }

  }
}