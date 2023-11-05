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
            struct VertexShader
            {
                wrl::com_ptr<ID3DBlob> vertex_shader;
            };
        }

        class VertexShaderResource : BaseResource<resources::VertexShader>
        {
        public:
            VertexShaderResource(const wrl::com_ptr<ID3DBlob>& vs)
                :m_vertex_shader({ vs })
            {}
            ~VertexShaderResource() override = default;

            resources::VertexShader* get()
            {
                return &m_vertex_shader;
            }
            const resources::VertexShader* get() const
            {
                return &m_vertex_shader;
            }

        private:
            resources::VertexShader m_vertex_shader;
        };
    }
}