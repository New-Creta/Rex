#pragma once

#include "rex_directx/system/dx_resource.h"
#include "rex_directx/system/dx_descriptor_heap.h"

#include "rex_renderer_core/resources/constant_buffer.h"

namespace rex
{
  namespace rhi
  {
    class DxConstantBuffer : public ConstantBuffer
    {
    public:
      DxConstantBuffer(const wrl::ComPtr<ID3D12Resource>& resource, DescriptorHandle handle, rsl::memory_size size)
        : ConstantBuffer(size)
        , m_resource(resource)
        , m_handle(handle)
      {}

      ID3D12Resource* dx_object()
      {
        return m_resource.Get();
      }

    private:
      wrl::ComPtr<ID3D12Resource> m_resource;
      DescriptorHandle m_handle;
    };
  } // namespace rhi
} // namespace rex