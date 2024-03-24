#pragma once

#include "rex_directx/utility/directx_util.h"
#include "rex_engine/engine/types.h"
#include "rex_renderer_core/resource_management/resource.h"
#include "rex_renderer_core/rendering/texture_format.h"

namespace rex
{
    namespace rhi
    {
        namespace resources
        {
            struct RenderTarget
            {
                wrl::ComPtr<ID3D12Resource> render_target{};
                renderer::TextureFormat format;
                s32 array_index{};
            };
        } // namespace resources

        class RenderTargetResource : public BaseResource<resources::RenderTarget>
        {
        public:
            RenderTargetResource(ResourceHash hash, const wrl::ComPtr<ID3D12Resource>& rt, renderer::TextureFormat format, s32 arrayIndex)
              : BaseResource(&m_render_target, hash)
                , m_render_target({rt, format, arrayIndex})
            {}
            ~RenderTargetResource() override = default;

        private:
            resources::RenderTarget m_render_target;
        };
    } // namespace renderer
} // namespace rex