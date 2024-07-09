#pragma once

#include "rex_directx/utility/dx_util.h"
#include "rex_directx/system/dx_descriptor_heap.h"

#include "rex_renderer_core/resources/render_target.h"

namespace rex
{
  namespace gfx
  {
    class DxRenderTarget : public RenderTarget
    {
    public:
      DxRenderTarget(const wrl::ComPtr<ID3D12Resource>& resource, DxResourceView descHandle);

      ID3D12Resource* dx_object();
      DxResourceView handle() const;

    private:
      wrl::ComPtr<ID3D12Resource> m_resource; // The resource itself
      DxResourceView m_desc_handle;         // A handle to the cpu and gpu descriptor
    };
  }
}