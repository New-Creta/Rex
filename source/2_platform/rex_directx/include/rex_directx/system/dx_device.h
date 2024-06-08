#pragma once

#include "rex_directx/dxgi/adapter.h"
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

      ID3D12Device1* get();
      const dxgi::Adapter* adapter() const;
      D3D_FEATURE_LEVEL feature_level() const;
      const Info& info() const;

      s64 memory_usage() const;
      s64 memory_available() const;

    private:
      void init_shader_model();
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
