#pragma once

#include "rex_renderer_core/resource_management/resource.h"
#include "rex_directx/utility/directx_util.h"
#include "rex_directx/dxgi/adapter.h"

namespace rex
{
  namespace rhi
  {
    class DirectXDevice
    {
    public:
      DirectXDevice(const wrl::ComPtr<ID3D12Device1>& device, D3D_FEATURE_LEVEL featureLevel, const dxgi::Adapter* adapter);

      ID3D12Device1* get();
      const dxgi::Adapter* adapter() const;
      D3D_FEATURE_LEVEL feature_level() const;

      s32 memory_usage() const;
      s32 memory_available() const;

    private:
      void init_shader_model();

    private:
      wrl::ComPtr<ID3D12Device1> m_device;
      const dxgi::Adapter* m_adapter;
      D3D_FEATURE_LEVEL m_feature_level;
      rsl::small_stack_string m_shader_version;
      rsl::small_stack_string m_api_version;
    };
  }
}
