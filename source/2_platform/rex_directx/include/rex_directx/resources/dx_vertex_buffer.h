#pragma once

#include "rex_directx/system/dx_resource.h"

#include "rex_renderer_core/resources/vertex_buffer.h"

namespace rex
{
  namespace rhi
  {
    class DxVertexBuffer : public VertexBuffer
    {
    public:
      DxVertexBuffer(const wrl::ComPtr<ID3D12Resource>& resource, s32 numVertices, rsl::memory_size vertexSize)
        : VertexBuffer(numVertices, vertexSize)
        , m_resource(resource)
      {
      }

      ID3D12Resource* dx_object()
      {
        return m_resource.Get();
      }

    private:
      wrl::ComPtr<ID3D12Resource> m_resource;
    };
  } // namespace rhi
} // namespace rex