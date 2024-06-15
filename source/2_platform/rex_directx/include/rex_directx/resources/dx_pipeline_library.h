#pragma once

#include "rex_directx/resources/dx_pipeline_state.h"
#include "rex_directx/utility/dx_util.h"

namespace rex
{
  namespace rhi
  {
    class PipelineLibrary
    {
    public:
      PipelineLibrary(const wrl::ComPtr<ID3D12PipelineLibrary>& pipelineLibrary, const wrl::ComPtr<ID3D12Device1>& device);

      // Load a pso from the library
      rsl::unique_ptr<PipelineState> load_pso(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc) const;
      // Store a pso into the library
      rsl::unique_ptr<PipelineState> store_pso(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);

    private:
      // Create a new unique pso name to use as key for a stored pso in the library
      rsl::wstring create_pso_name(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc) const;

    private:
      wrl::ComPtr<ID3D12PipelineLibrary> m_pipeline_library;
      wrl::ComPtr<ID3D12Device1> m_device;
    };
  } // namespace rhi
} // namespace rex