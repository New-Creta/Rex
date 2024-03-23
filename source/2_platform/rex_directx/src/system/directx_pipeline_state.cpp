#include "rex_directx/system/directx_pipeline_state.h"

namespace rex
{
  namespace rhi
  {
    PipelineState::PipelineState(const wrl::ComPtr<ID3D12PipelineState>& pso)
      : BaseResource(m_pso.Get())
      , m_pso(pso)
    {

    }
  }
}