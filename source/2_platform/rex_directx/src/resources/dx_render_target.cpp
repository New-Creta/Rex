#include "rex_directx/system/dx_render_target.h"

namespace rex
{
  namespace rhi
  {
    RenderTarget::RenderTarget(const wrl::ComPtr<ID3D12Resource>& resource, DescriptorHandle descHandle)
      : Resource2(resource)
      , m_desc_handle(descHandle)
    {

    }

    D3D12_CPU_DESCRIPTOR_HANDLE RenderTarget::cpu_handle()
    {
      return m_desc_handle.get();
    }

  }
}