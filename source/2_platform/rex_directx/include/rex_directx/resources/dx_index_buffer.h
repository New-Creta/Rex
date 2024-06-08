#pragma once



#include "rex_renderer_core/rhi/index_buffer_format.h"

#include "rex_renderer_core/resources/index_buffer.h"

namespace rex
{
  namespace rhi
  {
    class DxIndexBuffer : public IndexBuffer
    {
    public:
      DxIndexBuffer(const wrl::ComPtr<ID3D12Resource>& resource, s32 numIndices, renderer::IndexBufferFormat format)
        : IndexBuffer(numIndices, format)
        , m_resource(resource)
      {}


      ID3D12Resource* dx_object()
      {
        return m_resource.Get();
      }

    private:
      wrl::ComPtr<ID3D12Resource> m_resource;
    };

  } // namespace rhi
} // namespace rex
