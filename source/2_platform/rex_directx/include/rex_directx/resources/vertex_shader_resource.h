#pragma once

#include "rex_renderer_core/resource.h"
#include "rex_directx/directx_util.h"
#include "rex_engine/win/win_com_ptr.h"

namespace rex
{
    namespace renderer
    {
        namespace resources
        {
            struct VertexShader
            {
                wrl::ComPtr<ID3DBlob> vertex_shader;
            };
        }

        class VertexShaderResource : public BaseResource<resources::VertexShader>
        {
        public:
            RESOURCE_CLASS_TYPE(VertexShaderResource);

            VertexShaderResource(const wrl::ComPtr<ID3DBlob>& vs)
              : BaseResource(&m_vertex_shader)
                ,m_vertex_shader({ vs })
            {}
            ~VertexShaderResource() override = default;

        private:
            resources::VertexShader m_vertex_shader;
        };
    }
}