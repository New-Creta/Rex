#pragma once

#include "rex_directx/system/dx_descriptor_heap.h"

#include "rex_renderer_core/resources/texture_2d.h"

namespace rex
{
  namespace rhi
  {
    class DxTexture2D : public Texture2D
    {
    public:
      DxTexture2D(const wrl::ComPtr<ID3D12Resource>& resource, DescriptorHandle handle, s32 width, s32 height, renderer::TextureFormat format);

      ID3D12Resource* dx_object();
      D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle();

    private:
      wrl::ComPtr<ID3D12Resource> m_resource; // The resource itself
      DescriptorHandle m_desc_handle;         // A handle to the cpu and gpu descriptor
    };
  }
}