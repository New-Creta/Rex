#pragma once

#include "rex_directx/utility/directx_util.h"
#include "rex_engine/platform/win/win_com_ptr.h"
#include "rex_renderer_core/resource_management/resource.h"

namespace rex
{
    namespace rhi
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
            explicit VertexShaderResource(ResourceHash hash, const wrl::ComPtr<ID3DBlob>& vs)
              : BaseResource(&m_vertex_shader, hash)
                ,m_vertex_shader({ vs })
            {}
            ~VertexShaderResource() override = default;

        private:
            resources::VertexShader m_vertex_shader;
        };
    } // namespace renderer
} // namespace rex