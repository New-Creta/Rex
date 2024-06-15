#pragma once

#include "rex_renderer_core/resources/vertex_buffer.h"

#include "rex_directx/utility/dx_util.h"

namespace rex
{
  namespace rhi
  {
    class DxVertexBuffer : public VertexBuffer
    {
    public:
      DxVertexBuffer(const wrl::ComPtr<ID3D12Resource>& resource, s32 numVertices, rsl::memory_size vertexSize);

      ID3D12Resource* dx_object();

    private:
      wrl::ComPtr<ID3D12Resource> m_resource;
    };
  } // namespace rhi
} // namespace rex