#include "rex_directx/system/dx_pipeline_library.h"

#include "rex_std/bonus/functional.h"

#include "rex_directx/diagnostics/dx_call.h"

namespace rex
{
  namespace rhi
  {
    PipelineLibrary::PipelineLibrary(const wrl::ComPtr<ID3D12PipelineLibrary>& pipelineLibrary, const wrl::ComPtr<ID3D12Device1>& device)
      : m_pipeline_library(pipelineLibrary)
      , m_device(device)
    {}

    rsl::unique_ptr<PipelineState> PipelineLibrary::load_pso(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc) const
    {
      const rsl::wstring name = create_pso_name(desc);
      wrl::ComPtr<ID3D12PipelineState> pso;
      if (DX_FAILED(m_pipeline_library->LoadGraphicsPipeline(name.c_str(), &desc, IID_PPV_ARGS(&pso))))
      {
        return nullptr;
      }

      ResourceID id = rsl::hash<rsl::wstring>{}(name);
      return rsl::make_unique<PipelineState>(pso, id);
    }
    rsl::unique_ptr<PipelineState> PipelineLibrary::store_pso(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc)
    {
      wrl::ComPtr<ID3D12PipelineState> pso;
      m_device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pso));
      const rsl::wstring name = create_pso_name(desc);
      m_pipeline_library->StorePipeline(name.c_str(), pso.Get());
      ResourceID id = rsl::hash<rsl::wstring>{}(name);
      return rsl::make_unique<PipelineState>(pso, id);
    }

    rsl::wstring PipelineLibrary::create_pso_name(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc) const
    {
      const char* data = reinterpret_cast<const char*>(&desc);
      const size_t size = sizeof(desc);
      u32 hash = rsl::crc32c::Crc32c(data, size);
      return rsl::to_wstring(hash);
    }

  }
}