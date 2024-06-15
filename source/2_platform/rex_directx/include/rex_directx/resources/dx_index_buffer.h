#pragma once

#include "rex_renderer_core/rhi/index_buffer_format.h"

#include "rex_renderer_core/resources/index_buffer.h"
#include "rex_directx/utility/dx_util.h"

namespace rex
{
  namespace rhi
  {
    class DxIndexBuffer : public IndexBuffer
    {
    public:
      DxIndexBuffer(const wrl::ComPtr<ID3D12Resource>& resource, s32 numIndices, renderer::IndexBufferFormat format);

      ID3D12Resource* dx_object();

    private:
      wrl::ComPtr<ID3D12Resource> m_resource;
    };

  } // namespace rhi
} // namespace rex
