#pragma once

#include "rex_renderer_core/resource_management/resource.h"
#include "rex_directx/utility/directx_util.h"
#include "rex_directx/dxgi/adapter.h"

namespace rex
{
  namespace renderer
  {
    class DirectXDevice : public BaseResource<ID3D12Device>
    {
    public:
      RESOURCE_CLASS_TYPE(DirectXDevice);

      DirectXDevice(const wrl::ComPtr<ID3D12Device> device, D3D_FEATURE_LEVEL featureLevel, const dxgi::Adapter* adapter)
        : BaseResource(device.Get())
        , m_device(device)
        , m_adapter(adapter)
        , m_feature_level(featureLevel)
      {}

      const dxgi::Adapter* adapter() const
      {
        return m_adapter;
      }

      D3D_FEATURE_LEVEL feature_level() const
      {
        return m_feature_level;
      }

    private:
      wrl::ComPtr<ID3D12Device> m_device;
      const dxgi::Adapter* m_adapter;
      D3D_FEATURE_LEVEL m_feature_level;
    };
  }
}
