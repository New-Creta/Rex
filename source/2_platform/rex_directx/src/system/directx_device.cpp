#include "rex_directx/system/directx_device.h"

#include "rex_directx/system/directx_feature_shader_model.h"
#include "rex_directx/system/directx_feature_level.h"

namespace rex
{
  namespace rhi
  {
    DirectXDevice::DirectXDevice(const wrl::ComPtr<ID3D12Device> device, D3D_FEATURE_LEVEL featureLevel, const dxgi::Adapter* adapter)
      : BaseResource(device.Get())
      , m_device(device)
      , m_adapter(adapter)
      , m_feature_level(featureLevel)
    {
    }

    const dxgi::Adapter* DirectXDevice::adapter() const
    {
      return m_adapter;
    }
    D3D_FEATURE_LEVEL DirectXDevice::feature_level() const
    {
      return m_feature_level;
    }

    s32 DirectXDevice::memory_usage() const
    {
      DXGI_QUERY_VIDEO_MEMORY_INFO info;
      m_adapter->as<IDXGIAdapter3>()->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &info);
      return info.CurrentUsage;
    }
    s32 DirectXDevice::memory_available() const
    {
      DXGI_QUERY_VIDEO_MEMORY_INFO info;
      m_adapter->as<IDXGIAdapter3>()->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &info);
      return info.Budget;
    }

    void DirectXDevice::init_shader_model()
    {
      const D3D_SHADER_MODEL shader_model = query_shader_model_version(m_device.Get());

      m_shader_version = shader_model_name(shader_model);
      m_api_version = feature_level_name(m_feature_level);
    }
  }
}