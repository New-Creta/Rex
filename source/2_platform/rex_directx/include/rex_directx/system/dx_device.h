#pragma once

#include "rex_directx/dxgi/dx_adapter.h"
#include "rex_directx/utility/dx_util.h"
#include "rex_renderer_core/resource_management/resource.h"
#include "rex_renderer_core/rhi/rhi.h"

namespace rex
{
  namespace rhi
  {
    class DxDevice
    {
    public:
      DxDevice(const wrl::ComPtr<ID3D12Device1>& device, D3D_FEATURE_LEVEL featureLevel, const dxgi::Adapter* adapter);

      // Return the wrapped directx device
      ID3D12Device1* get();
      // Return the adapter this device is used for
      const dxgi::Adapter* adapter() const;
      // Return the feature level of this device
      D3D_FEATURE_LEVEL feature_level() const;
      // Return some general information of this device
      const Info& info() const;

      // Return the current memory usage of this device
      s64 memory_usage() const;
      // Return the amount of left available memory of this device
      s64 memory_available() const;
      // Return the total memory of this device
      s64 memory_total() const;

    private:
      // Query the shader model of this device and cache it
      void init_shader_model();
      // Query the driver version of this device on the current machine
      rsl::small_stack_string query_driver_version() const;

    private:
      wrl::ComPtr<ID3D12Device1> m_device;
      const dxgi::Adapter* m_adapter;
      D3D_FEATURE_LEVEL m_feature_level;
      Info m_info;
      rsl::small_stack_string m_shader_version;
      rsl::small_stack_string m_api_version;
    };
  } // namespace rhi
} // namespace rex
