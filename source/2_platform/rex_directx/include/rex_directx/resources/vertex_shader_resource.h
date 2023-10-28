#pragma once

#include "rex_directx/resources/resource.h"
#include "rex_directx/directx_util.h"
#include "rex_directx/wrl/wrl_types.h"

namespace rex
{
    namespace renderer
    {
        class VertexShaderResource : BaseResource<ID3DBlob>
        {
        public:
            VertexShaderResource(const wrl::com_ptr<ID3DBlob>& vs)
                :m_vertex_shader(vs)
            {}
            ~VertexShaderResource() override = default;

            const ID3DBlob* get() const
            {
                return m_vertex_shader.Get();
            }

        private:
            wrl::com_ptr<ID3DBlob> m_vertex_shader;
        };
    }
}