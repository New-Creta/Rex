#pragma once

#include "rex_directx/utility/dx_util.h"
#include "rex_renderer_core/resource_management/resource.h"

namespace rex
{
  namespace rhi
  {
    class PipelineState : public BaseResource<ID3D12PipelineState>
    {
    public:
      PipelineState(const wrl::ComPtr<ID3D12PipelineState>& pso, ResourceID id);

    private:
      wrl::ComPtr<ID3D12PipelineState> m_pso;
    };
  } // namespace rhi
} // namespace rex