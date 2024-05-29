#include "rex_directx/system/dx_texture_2d.h"

namespace rex
{
  namespace rhi
  {
    DxTexture2D::DxTexture2D(const wrl::ComPtr<ID3D12Resource>& texture, DescriptorHandle handle, s32 width, s32 height, renderer::TextureFormat format)
      : Resource2(texture)
      , m_handle(handle)
      , m_width(width)
      , m_height(height)
      , m_format(format)
    {}

    D3D12_GPU_DESCRIPTOR_HANDLE DxTexture2D::gpu_handle()
    {
      return m_handle.get_gpu();
    }

  }
}