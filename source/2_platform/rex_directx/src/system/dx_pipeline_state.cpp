#include "rex_directx/system/dx_pipeline_state.h"

namespace rex
{
  namespace rhi
  {
    PipelineState::PipelineState(const wrl::ComPtr<ID3D12PipelineState>& pso)
      : BaseResource(pso.Get(), ResourceHash(pso.Get()))
      , m_pso(pso)
    {

    }
  }
}