#include "rex_directx/resources/dx_render_target.h"

#include "rex_directx/utility/dx_util.h"

namespace rex
{
  namespace gfx
  {
    DxRenderTarget::DxRenderTarget(const wrl::ComPtr<ID3D12Resource>& resource, DescriptorHandle descHandle)
      : RenderTarget(static_cast<s32>(resource->GetDesc().Width), static_cast<s32>(resource->GetDesc().Height))
      , m_resource(resource)
      , m_desc_handle(descHandle)
    {}

    ID3D12Resource* DxRenderTarget::dx_object()
    {
      return m_resource.Get();
    }

    DescriptorHandle DxRenderTarget::handle() const
    {
      return m_desc_handle;
    }
  }
}