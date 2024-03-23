#pragma once

#include "rex_directx/utility/directx_util.h"
#include "rex_engine/engine/types.h"
#include "rex_renderer_core/resource_management/resource.h"

namespace rex
{
    namespace rhi
    {
        namespace resources
        {
            struct DepthStencilTarget
            {
                wrl::ComPtr<ID3D12Resource> render_target{};
                s32 array_index{};
            };
        } // namespace resources

        class DepthStencilTargetResource : public BaseResource<resources::DepthStencilTarget>
        {
        public:
            DepthStencilTargetResource(const wrl::ComPtr<ID3D12Resource>& dst, s32 arrayIndex)
                : BaseResource(&m_depth_stencil_target)
                , m_depth_stencil_target({ dst, arrayIndex })
            {}
            ~DepthStencilTargetResource() override = default;

        private:
            resources::DepthStencilTarget m_depth_stencil_target;
        };
    } // namespace renderer
} // namespace rex