#include "rex_directx/system/directx_resource.h"

namespace rex
{
  namespace rhi
  {
    Resource::Resource(const wrl::ComPtr<ID3D12Resource>& resource)
      : m_resource(resource)
    {
      D3D12_RESOURCE_DESC desc = m_resource->GetDesc();
      m_width = desc.Width;
      m_height = desc.Height;
      m_format = desc.Format;
    }

    s32 Resource::width() const
    {
      return m_width;
    }
    s32 Resource::height() const
    {
      return m_height;
    }
    DXGI_FORMAT Resource::format() const
    {
      return m_format;
    }
    ID3D12Resource* Resource::get() const
    {
      return m_resource.Get();
    }
  }
}