#pragma once

#include "rex_directx/directx_util.h"
#include "rex_directx/resources/resource.h"
#include "rex_engine/types.h"

namespace rex
{
    namespace renderer
    {
        namespace resources
        {
            struct DepthStencilTarget
            {
                wrl::com_ptr<ID3D12Resource> render_target;
                s32 array_index;
            };
        } // namespace resources

        class DepthStencilTargetResource : public BaseResource<resources::DepthStencilTarget>
        {
        public:
            RESOURCE_CLASS_TYPE(DepthStencilTargetResource);

            DepthStencilTargetResource(const wrl::com_ptr<ID3D12Resource>& dst, s32 arrayIndex)
                : m_depth_stencil_target({ dst, arrayIndex })
            {}
            ~DepthStencilTargetResource() override = default;

            resources::DepthStencilTarget* get()
            {
                return &m_depth_stencil_target;
            }
            const resources::DepthStencilTarget* get() const
            {
                return &m_depth_stencil_target;
            }

        private:
            resources::DepthStencilTarget m_depth_stencil_target;
        };
    } // namespace renderer
} // namespace rex