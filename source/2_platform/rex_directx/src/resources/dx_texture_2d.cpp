#include "rex_directx/resources/dx_texture_2d.h"

namespace rex
{
  namespace gfx
  {
    DxTexture2D::DxTexture2D(const wrl::ComPtr<ID3D12Resource>& texture, DxResourceView handle, s32 width, s32 height, TextureFormat format)
      : Texture2D(texture->GetGPUVirtualAddress(), &m_view, width, height, format)
      , m_resource(texture)
      , m_view(handle)
    {}

    // Return a view to the object on the gpu
    ID3D12Resource* DxTexture2D::dx_object()
    {
      return m_resource.Get();
    }
    // Return the wrapped DirectX object
    DxResourceView* DxTexture2D::view()
    {
      return &m_view;
    }

  }
}