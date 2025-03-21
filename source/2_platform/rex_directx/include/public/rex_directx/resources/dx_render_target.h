#pragma once

#include "rex_directx/utility/dx_util.h"
#include "rex_directx/system/dx_view_heap.h"

#include "rex_engine/gfx/resources/render_target.h"

namespace rex
{
  namespace gfx
  {
    class DxRenderTarget : public RenderTarget
    {
    public:
      DxRenderTarget(const wrl::ComPtr<ID3D12Resource>& resource, DxResourceView descHandle, const ClearStateDesc& clearState);

      // Return the wrapped DirectX object
      ID3D12Resource* dx_object();
      // Return a view to the object
      DxResourceView view() const;

    private:
      wrl::ComPtr<ID3D12Resource> m_resource; // The resource itself
      DxResourceView m_view;         // A handle to the cpu and gpu descriptor
    };
  }
}