#pragma once

#include "rex_directx/utility/dx_util.h"
#include "rex_directx/system/dx_view_heap.h"

#include "rex_engine/gfx/resources/render_target.h"
#include "rex_directx/resources/dx_resource.h"

namespace rex
{
  namespace gfx
  {
    class DxRenderTarget : public RenderTarget, public DxResource
    {
    public:
      DxRenderTarget(const wrl::ComPtr<ID3D12Resource>& resource, DxResourceView view, const rsl::Color4f& clearColor);

      void debug_set_name(rsl::string_view name) override;

      // Return a view to the object
      const DxResourceView& dx_view() const;

    private:
      wrl::ComPtr<ID3D12Resource> m_resource; // The resource itself
      DxResourceView m_view;
    };
  }
}