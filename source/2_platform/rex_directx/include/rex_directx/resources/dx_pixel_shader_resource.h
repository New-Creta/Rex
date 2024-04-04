#pragma once

#include "rex_directx/utility/dx_util.h"
#include "rex_engine/platform/win/win_com_ptr.h"
#include "rex_renderer_core/resource_management/resource.h"

namespace rex
{
    namespace rhi
    {
        namespace resources
        {
            struct PixelShader
            {
                wrl::ComPtr<ID3DBlob> pixel_shader{};
            };
        } // namespace resources

        class PixelShaderResource : public BaseResource<resources::PixelShader>
        {
        public:
            explicit PixelShaderResource(ResourceHash hash, const wrl::ComPtr<ID3DBlob>& ps)
              : BaseResource(&m_pixel_shader, hash)
                ,m_pixel_shader({ ps })
            {}
            ~PixelShaderResource() override = default;

        private:
            resources::PixelShader m_pixel_shader;
        };
    } // namespace renderer
} // namespace rex