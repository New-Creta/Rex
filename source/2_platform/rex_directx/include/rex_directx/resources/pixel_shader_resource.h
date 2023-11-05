#pragma once

#include "rex_directx/resources/resource.h"
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

        class PixelShaderResource : BaseResource<resources::PixelShader>
        {
        public:
            PixelShaderResource(const wrl::com_ptr<ID3DBlob>& ps)
                :m_pixel_shader({ ps })
            {}
            ~PixelShaderResource() override = default;

            resources::PixelShader* get()
            {
                return &m_pixel_shader;
            }
            const resources::PixelShader* get() const
            {
                return &m_pixel_shader;
            }

        private:
            resources::PixelShader m_pixel_shader;
        };
    }
}