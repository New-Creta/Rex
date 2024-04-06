#include "rex_directx/system/dx_pipeline_state.h"

namespace rex
{
  namespace rhi
  {
    PipelineState::PipelineState(const wrl::ComPtr<ID3D12PipelineState>& pso, ResourceHash hash)
        : BaseResource(pso.Get(), hash)
        , m_pso(pso)
    {
    }
  } // namespace rhi
} // namespace rex