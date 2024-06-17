#include "rex_directx/resources/dx_pipeline_state.h"

namespace rex
{
  namespace gfx
  {
    DxPipelineState::DxPipelineState(const wrl::ComPtr<ID3D12PipelineState>& pso)
      : m_pso(pso)
    {

    }

    ID3D12PipelineState* DxPipelineState::dx_object()
    {
      return m_pso.Get();
    }

  }
}