#pragma once

#include "rex_directx/system/dx_view_heap.h"

#include "rex_renderer_core/resources/texture_2d.h"

#include "rex_renderer_core/system/resource_view.h"

namespace rex
{
  namespace gfx
  {
    class DxTexture2D : public Texture2D
    {
    public:
      DxTexture2D(const wrl::ComPtr<ID3D12Resource>& resource, DxResourceView handle, s32 width, s32 height, TextureFormat format);

      // Return the wrapped DirectX object
      ID3D12Resource* dx_object();
      // Return a view to the object on the gpu
      DxResourceView* view();

    private:
      wrl::ComPtr<ID3D12Resource> m_resource; // The resource itself
      DxResourceView m_desc_handle;         // A handle to the cpu and gpu descriptor
    };
  }
}