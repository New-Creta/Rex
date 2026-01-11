#pragma once

#include "rex_directx/system/dx_view_heap.h"

#include "rex_engine/gfx/resources/texture_2d.h"
#include "rex_directx/resources/dx_resource.h"

#include "rex_engine/gfx/system/resource_view.h"

namespace rex
{
  namespace gfx
  {
    class DxTexture2D : public Texture2D, public DxResource
    {
    public:
      DxTexture2D(const wrl::ComPtr<ID3D12Resource>& resource, DxResourceView handle, s32 width, s32 height, TextureFormat format);

      // Return a view to the object on the gpu
      DxResourceView* view();

      void* api_object() const override;

    private:
      DxResourceView m_view;         // A handle to the cpu and gpu descriptor
    };
  }
}