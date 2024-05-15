#include "rex_directx/system/dx_pipeline_state.h"

namespace rex
{
  namespace rhi
  {
    PipelineState::PipelineState(const wrl::ComPtr<ID3D12PipelineState>& pso)
      : m_pso(pso)
    {

    }

    ID3D12PipelineState* PipelineState::get()
    {
      return m_pso.Get();
    }

  }
}