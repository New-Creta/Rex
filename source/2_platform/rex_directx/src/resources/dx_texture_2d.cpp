#include "rex_directx/resources/dx_texture_2d.h"

namespace rex
{
  namespace rhi
  {
    DxTexture2D::DxTexture2D(const wrl::ComPtr<ID3D12Resource>& texture, DescriptorHandle handle, s32 width, s32 height, renderer::TextureFormat format)
      : Texture2D(width, height, format)
      , m_resource(texture)
      , m_handle(handle)
    {}

    D3D12_GPU_DESCRIPTOR_HANDLE DxTexture2D::gpu_handle()
    {
      return m_handle.gpu_handle();
    }

    ID3D12Resource* DxTexture2D::dx_object()
    {
      return m_resource.Get();
    }
  }
}