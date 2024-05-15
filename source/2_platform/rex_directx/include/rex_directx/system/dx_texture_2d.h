#pragma once

#include "rex_directx/system/dx_resource.h"
#include "rex_directx/system/dx_descriptor_heap.h"

namespace rex
{
  namespace rhi
  {
    class Texture2D : public Resource2
    {
    public:
      Texture2D(const wrl::ComPtr<ID3D12Resource>& resource, DescriptorHandle handle, s32 width, s32 height, renderer::TextureFormat format);

      D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle();

    private:
      DescriptorHandle m_handle;
      s32 m_width;
      s32 m_height;
      renderer::TextureFormat m_format;

    };
  }
}