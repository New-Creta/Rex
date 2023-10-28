#pragma once

#include "rex_directx/resources/resource.h"
#include "rex_directx/directx_util.h"
#include "rex_directx/wrl/wrl_types.h"

namespace rex
{
    namespace renderer
    {
        class PixelShaderResource : BaseResource<ID3DBlob>
        {
        public:
            PixelShaderResource(const wrl::com_ptr<ID3DBlob>& ps)
                :m_pixel_shader(ps)
            {}
            ~PixelShaderResource() override = default;

            const ID3DBlob* get() const
            {
                return m_pixel_shader.Get();
            }

        private:
            wrl::com_ptr<ID3DBlob> m_pixel_shader;
        };
    }
}