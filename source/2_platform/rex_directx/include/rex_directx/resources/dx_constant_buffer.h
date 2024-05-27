#pragma once

#include "rex_directx/system/dx_resource.h"
#include "rex_directx/system/dx_descriptor_heap.h"

namespace rex
{
  namespace rhi
  {
    class ConstantBuffer : public Resource2
    {
    public:
      ConstantBuffer(const wrl::ComPtr<ID3D12Resource>& resource, DescriptorHandle handle, rsl::memory_size size)
        : Resource2(resource)
        , m_handle(handle)
        , m_size(size)
      {}

    private:
      DescriptorHandle m_handle;
      rsl::memory_size m_size;
    };
  } // namespace rhi
} // namespace rex