#pragma once

#include "rex_directx/utility/dx_util.h"

namespace rex
{
  namespace resources
  {
    class RenderTarget
    {
    public:
      RenderTarget(const wrl::ComPtr<ID3D12Resource>& resource, s32 width, s32 height);

    private:
      s32 m_width;
      s32 m_height;
      wrl::ComPtr<ID3D12Resource> m_resource;
    };
  }
}