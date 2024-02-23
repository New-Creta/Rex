#pragma once

#include "rex_directx/directx_util.h"
#include "rex_engine/engine/types.h"
#include "rex_renderer_core/resource.h"
#include "rex_renderer_core/texture_format.h"

namespace rex
{
    namespace renderer
    {
        namespace resources
        {
            struct RenderTarget
            {
                wrl::ComPtr<ID3D12Resource> render_target{};
                TextureFormat format;
                s32 array_index{};
            };
        } // namespace resources

        class RenderTargetResource : public BaseResource<resources::RenderTarget>
        {
        public:
            RESOURCE_CLASS_TYPE(RenderTargetResource);

            RenderTargetResource(const wrl::ComPtr<ID3D12Resource>& rt, TextureFormat format, s32 arrayIndex)
              : BaseResource(&m_render_target)
                , m_render_target({rt, format, arrayIndex})
            {}
            ~RenderTargetResource() override = default;

        private:
            resources::RenderTarget m_render_target;
        };
    } // namespace renderer
} // namespace rex