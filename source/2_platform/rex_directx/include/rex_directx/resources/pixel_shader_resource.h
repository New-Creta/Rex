#pragma once

#include "rex_renderer_core/resource.h"
#include "rex_directx/directx_util.h"
#include "rex_directx/wrl/wrl_types.h"

namespace rex
{
    namespace renderer
    {
        namespace resources
        {
            struct PixelShader
            {
                wrl::com_ptr<ID3DBlob> pixel_shader;
            };
        }

        class PixelShaderResource : public BaseResource<resources::PixelShader>
        {
        public:
            RESOURCE_CLASS_TYPE(PixelShaderResource);

            PixelShaderResource(const wrl::com_ptr<ID3DBlob>& ps)
              : BaseResource(&m_pixel_shader)
                ,m_pixel_shader({ ps })
            {}
            ~PixelShaderResource() override = default;

        private:
            resources::PixelShader m_pixel_shader;
        };
    }
}