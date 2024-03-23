#pragma once

#include "rex_directx/utility/directx_util.h"

#include "rex_renderer_core/resource_management/resource.h"

#include "rex_engine/engine/types.h"

namespace rex
{
  namespace rhi
  {
    class Resource : public BaseResource<Resource>
    {
    public:
      explicit Resource(const wrl::ComPtr<ID3D12Resource>& resource);

      s32 width() const;
      s32 height() const;
      DXGI_FORMAT format() const;
      ID3D12Resource* get() const;

    private:
      wrl::ComPtr<ID3D12Resource> m_resource;
      DXGI_FORMAT m_format;
      s32 m_width;
      s32 m_height;
    };
  }
}