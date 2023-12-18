#pragma once

#include "rex_directx/directx_util.h"
#include "rex_renderer_core/resource.h"
#include "rex_engine/types.h"

namespace rex
{
    namespace renderer
    {
        namespace resources
        {
            struct RenderTarget
            {
                wrl::com_ptr<ID3D12Resource> render_target;
                s32 array_index;
            };
        } // namespace resources

        class RenderTargetResource : public BaseResource<resources::RenderTarget>
        {
        public:
            RESOURCE_CLASS_TYPE(RenderTargetResource);

            RenderTargetResource(const wrl::com_ptr<ID3D12Resource>& rt, s32 arrayIndex)
                : m_render_target({ rt, arrayIndex })
            {}
            ~RenderTargetResource() override = default;

            resources::RenderTarget* get()
            {
                return &m_render_target;
            }
            const resources::RenderTarget* get() const
            {
                return &m_render_target;
            }

        private:
            resources::RenderTarget m_render_target;
        };
    } // namespace renderer
} // namespace rex