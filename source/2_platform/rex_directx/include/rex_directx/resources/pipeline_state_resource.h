#pragma once

#include "rex_renderer_core/resource.h"
#include "rex_directx/directx_util.h"
#include "rex_engine/win/win_com_ptr.h"

namespace rex
{
    namespace renderer
    {
        class PipelineStateResource : public BaseResource<ID3D12PipelineState>
        {
        public:
            RESOURCE_CLASS_TYPE(PipelineStateResource);

            PipelineStateResource(const wrl::ComPtr<ID3D12PipelineState>& pso)
              : BaseResource(pso.Get())
                ,m_pipeline_state(pso)
            {}
            ~PipelineStateResource() override = default;

        private:
            wrl::ComPtr<ID3D12PipelineState> m_pipeline_state;
        };
    }
}