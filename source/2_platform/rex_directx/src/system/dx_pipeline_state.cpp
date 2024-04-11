#include "rex_directx/system/dx_pipeline_state.h"

namespace rex
{
  namespace rhi
  {
    PipelineState::PipelineState(const wrl::ComPtr<ID3D12PipelineState>& pso, ResourceID id)
      : BaseResource(pso.Get(), id)
      , m_pso(pso)
    {

    }
  }
}