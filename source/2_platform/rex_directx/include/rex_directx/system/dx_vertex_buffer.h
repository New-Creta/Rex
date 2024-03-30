#pragma once

#include "rex_directx/system/dx_resource.h"

namespace rex
{
  namespace rhi
  {
    class VertexBuffer : public Resource
    {
    public:
      VertexBuffer(const wrl::ComPtr<ID3D12Resource>& resource, s32 size, s32 stride)
        : Resource(resource, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, size)
        , m_stride(stride)
      {}

      s32 stride() const
      {
        return m_stride;
      }

    private:
      s32 m_stride;
    };
  }
}