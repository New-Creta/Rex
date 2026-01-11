#include "rex_directx/resources/dx_render_target.h"

#include "rex_directx/utility/dx_util.h"

namespace rex
{
  namespace gfx
  {
    DxRenderTarget::DxRenderTarget(const wrl::ComPtr<ID3D12Resource>& resource, DxResourceView view, const rsl::Color4f& clearColor)
      : RenderTarget(static_cast<s32>(resource->GetDesc().Width), static_cast<s32>(resource->GetDesc().Height), &m_view, clearColor)
      , DxResource(resource)
      , m_view(view)
    {
    }

    const DxResourceView& DxRenderTarget::dx_view() const
    {
      return m_view;
    }

    void* DxRenderTarget::api_object() const
    {
      return dx_object();
    }

  }
}