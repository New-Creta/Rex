#include "rex_directx/resources/dx_texture_2d.h"

namespace rex
{
  namespace gfx
  {
    DxTexture2D::DxTexture2D(const wrl::ComPtr<ID3D12Resource>& texture, DxResourceView handle, s32 width, s32 height, TextureFormat format)
      : Texture2D(width, height, format)
      , m_resource(texture)
      , m_desc_handle(handle)
    {}

    D3D12_GPU_DESCRIPTOR_HANDLE DxTexture2D::gpu_handle()
    {
      return m_desc_handle.gpu_handle();
    }

    DxResourceView* DxTexture2D::handle()
    {
      return &m_desc_handle;
    }

    ID3D12Resource* DxTexture2D::dx_object()
    {
      return m_resource.Get();
    }
  }
}