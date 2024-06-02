#include "rex_directx/resources/dx_pipeline_state.h"

namespace rex
{
  namespace rhi
  {
    DxPipelineState::DxPipelineState(const wrl::ComPtr<ID3D12PipelineState>& pso)
      : m_pso(pso)
    {

    }

    ID3D12PipelineState* DxPipelineState::get()
    {
      return m_pso.Get();
    }

  }
}