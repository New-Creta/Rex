#include "rex_directx/system/dx_texture_2d.h"

namespace rex
{
  namespace rhi
  {
    Texture2D::Texture2D(const wrl::ComPtr<ID3D12Resource>& texture, s32 width, s32 height, DXGI_FORMAT format)
      : Resource2(texture, D3D12_RESOURCE_STATE_COMMON, width, height, format)
    {}
  }
}