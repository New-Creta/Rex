#include "rex_directx/system/dx_device.h"

#include "rex_directx/system/dx_feature_level.h"
#include "rex_directx/system/dx_feature_shader_model.h"
#include "rex_directx/dxgi/includes.h"

namespace rex
{
  namespace rhi
  {
    DxDevice::DxDevice(const wrl::ComPtr<ID3D12Device1>& device, D3D_FEATURE_LEVEL featureLevel, const dxgi::Adapter* adapter)
        : m_device(device)
        , m_adapter(adapter)
        , m_feature_level(featureLevel)
        , m_info()
    {
      const D3D_SHADER_MODEL shader_model = query_shader_model_version(m_device.Get());

      m_info.api            = "DirectX";
      m_info.api_version    = feature_level_name(m_feature_level);
      m_info.shader_version = shader_model_name(shader_model);
      m_info.adaptor        = m_adapter->description().name;
      m_info.vendor         = m_adapter->description().vendor_name;
      m_info.driver_version = query_driver_version();
      m_info.available_memory = m_adapter->description().dedicated_video_memory;
    }

    ID3D12Device1* DxDevice::get()
    {
      return m_device.Get();
    }

    const dxgi::Adapter* DxDevice::adapter() const
    {
      return m_adapter;
    }
    D3D_FEATURE_LEVEL DxDevice::feature_level() const
    {
      return m_feature_level;
    }
    const Info& DxDevice::info() const
    {
      return m_info;
    }

    s64 DxDevice::memory_usage() const
    {
      DXGI_QUERY_VIDEO_MEMORY_INFO info;
      m_adapter->as<IDXGIAdapter3>()->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &info);
      return static_cast<s64>(info.CurrentUsage);
    }
    s64 DxDevice::memory_available() const
    {
      DXGI_QUERY_VIDEO_MEMORY_INFO info;
      m_adapter->as<IDXGIAdapter3>()->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &info);
      return static_cast<s64>(info.Budget);
    }
    s64 DxDevice::memory_total() const
    {
      return m_adapter->description().dedicated_video_memory;
    }

    void DxDevice::init_shader_model()
    {
      const D3D_SHADER_MODEL shader_model = query_shader_model_version(m_device.Get());

      m_shader_version = shader_model_name(shader_model);
      m_api_version    = feature_level_name(m_feature_level);
    }

    rsl::small_stack_string DxDevice::query_driver_version() const
    {
      return "Unknown"_small;
    }
  } // namespace rhi
} // namespace rex