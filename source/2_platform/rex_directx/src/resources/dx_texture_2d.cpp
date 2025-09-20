#include "rex_directx/resources/dx_texture_2d.h"

namespace rex
{
  namespace gfx
  {
    DxTexture2D::DxTexture2D(const wrl::ComPtr<ID3D12Resource>& texture, DxResourceView handle, s32 width, s32 height, TextureFormat format)
      : Texture2D(&m_view, width, height, format)
      , DxResource(texture)
      , m_view(handle)
    {}

    // Return the wrapped DirectX object
    DxResourceView* DxTexture2D::view()
    {
      return &m_view;
    }

    void* DxTexture2D::api_object()
    {
      return dx_object();
    }
  }
}