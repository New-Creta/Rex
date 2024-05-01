#pragma once

#include "rex_directx/system/dx_resource.h"

namespace rex
{
  namespace rhi
  {
    class Texture2D : public Resource
    {
    public:
      Texture2D(const wrl::ComPtr<ID3D12Resource>& resource, s32 width, s32 height, DXGI_FORMAT format);
    private:
    };
  }
}