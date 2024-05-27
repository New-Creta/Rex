#pragma once

#include "rex_directx/utility/dx_util.h"

#include "rex_directx/system/dx_descriptor_heap.h"
#include "rex_directx/system/dx_resource.h"

namespace rex
{
  namespace rhi
  {
    class RenderTarget : public Resource2
    {
    public:
      RenderTarget(const wrl::ComPtr<ID3D12Resource>& resource, DescriptorHandle descHandle);

      D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle();

    private:
      DescriptorHandle m_desc_handle;
    };
  }
}