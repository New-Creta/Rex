#pragma once

#include "rex_directx/utility/dx_util.h"

#include "rex_renderer_core/resources/render_target.h"

namespace rex
{
  namespace rhi
  {
    class DxRenderTarget : public RenderTarget
    {
    public:
      DxRenderTarget(const wrl::ComPtr<ID3D12Resource>& resource, s32 width, s32 height)
        : RenderTarget(width, height)
        , m_resource(resource)
      {}

    private:
      wrl::ComPtr<ID3D12Resource> m_resource;
    };
  }
}