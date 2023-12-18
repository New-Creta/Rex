#pragma once

#include "rex_renderer_core/resource.h"
#include "rex_directx/directx_util.h"
#include "rex_directx/wrl/wrl_types.h"

namespace rex
{
    namespace renderer
    {
        class PipelineStateResource : public BaseResource<ID3D12PipelineState>
        {
        public:
            RESOURCE_CLASS_TYPE(PipelineStateResource);

            PipelineStateResource(const wrl::com_ptr<ID3D12PipelineState>& pso)
                :m_pipeline_state(pso)
            {}
            ~PipelineStateResource() override = default;

            ID3D12PipelineState* get()
            {
                return m_pipeline_state.Get();
            }
            const ID3D12PipelineState* get() const
            {
                return m_pipeline_state.Get();
            }

        private:
            wrl::com_ptr<ID3D12PipelineState> m_pipeline_state;
        };
    }
}