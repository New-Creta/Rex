#pragma once


#include "rex_directx/system/dx_descriptor_heap.h"

#include "rex_renderer_core/resources/constant_buffer.h"

namespace rex
{
  namespace rhi
  {
    class DxConstantBuffer : public ConstantBuffer
    {
    public:
      DxConstantBuffer(const wrl::ComPtr<ID3D12Resource>& resource, DescriptorHandle handle, rsl::memory_size size);

      ID3D12Resource* dx_object();

    private:
      wrl::ComPtr<ID3D12Resource> m_resource; // the actual constant buffer resource
      DescriptorHandle m_handle; // A handle for both the cpu and gpu descriptor
    };
  } // namespace rhi
} // namespace rex