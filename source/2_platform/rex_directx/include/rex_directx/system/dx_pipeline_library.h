#pragma once

#include "rex_directx/utility/dx_util.h"
#include "rex_directx/system/dx_pipeline_state.h"

namespace rex
{
  namespace rhi
  {
    class PipelineLibrary
    {
    public:
      PipelineLibrary(const wrl::ComPtr<ID3D12PipelineLibrary>& pipelineLibrary, const wrl::ComPtr<ID3D12Device1>& device);

      rsl::unique_ptr<PipelineState> load_pso(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc) const;
      rsl::unique_ptr<PipelineState> store_pso(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);

    private:
      rsl::wstring create_pso_name(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc) const;

    private:
      wrl::ComPtr<ID3D12PipelineLibrary> m_pipeline_library;
      wrl::ComPtr<ID3D12Device1> m_device;
    };
  }
}