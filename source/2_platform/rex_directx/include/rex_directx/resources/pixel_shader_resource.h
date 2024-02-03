#pragma once

#include "rex_directx/directx_util.h"
#include "rex_engine/platform/win/win_com_ptr.h"
#include "rex_renderer_core/resource.h"

namespace rex
{
    namespace renderer
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
            RESOURCE_CLASS_TYPE(PixelShaderResource);

            explicit PixelShaderResource(const wrl::ComPtr<ID3DBlob>& ps)
              : BaseResource(&m_pixel_shader)
                ,m_pixel_shader({ ps })
            {}
            ~PixelShaderResource() override = default;

        private:
            resources::PixelShader m_pixel_shader;
        };
    } // namespace renderer
} // namespace rex