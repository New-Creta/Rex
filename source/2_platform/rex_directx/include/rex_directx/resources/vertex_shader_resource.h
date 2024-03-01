#pragma once

#include "rex_directx/directx_util.h"
#include "rex_engine/platform/win/win_com_ptr.h"
#include "rex_renderer_resources/resource.h"

namespace rex
{
    namespace renderer
    {
        namespace resources
        {
            struct VertexShader
            {
                wrl::ComPtr<ID3DBlob> vertex_shader{};
            };
        } // namespace resources

        class VertexShaderResource : public BaseResource<resources::VertexShader>
        {
        public:
            RESOURCE_CLASS_TYPE(VertexShaderResource);

            explicit VertexShaderResource(const wrl::ComPtr<ID3DBlob>& vs)
              : BaseResource(&m_vertex_shader)
                ,m_vertex_shader({ vs })
            {}
            ~VertexShaderResource() override = default;

        private:
            resources::VertexShader m_vertex_shader;
        };
    } // namespace renderer
} // namespace rex