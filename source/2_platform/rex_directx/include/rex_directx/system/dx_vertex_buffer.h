#pragma once

#include "rex_directx/system/dx_resource.h"

namespace rex
{
  namespace rhi
  {
    class VertexBuffer : public Resource2
    {
    public:
      VertexBuffer(const wrl::ComPtr<ID3D12Resource>& resource, rsl::memory_size vertexSize, rsl::memory_size stride)
        : Resource2(resource)
        , m_vertex_size(vertexSize)
        , m_stride(stride)
      {
      }

      s32 vertex_size() const
      {
        return m_vertex_size;
      }

      s32 stride() const
      {
        return m_stride;
      }

      s32 total_size() const
      {
        return m_vertex_size * m_stride;
      }

    private:
      s32 m_vertex_size;
      s32 m_stride;
    };
  } // namespace rhi
} // namespace rex