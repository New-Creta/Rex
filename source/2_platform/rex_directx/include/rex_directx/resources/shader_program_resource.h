#pragma once

#include "rex_directx/utility/directx_util.h"
#include "rex_engine/platform/win/win_com_ptr.h"
#include "rex_renderer_core/resource_management/resource.h"

#include "rex_renderer_core/commands/create_constant_layout_description_cmd.h"

namespace rex
{
    namespace rhi
    {
        namespace resources
        {
            struct ShaderProgram
            {
                wrl::ComPtr<ID3D12RootSignature> root_signature{};
                wrl::ComPtr<ID3DBlob> vertex_shader{};
                wrl::ComPtr<ID3DBlob> pixel_shader{};

                rsl::vector<renderer::commands::ConstantLayoutDescription> constants{};
            };
        } // namespace resources

        class ShaderProgramResource : public BaseResource<resources::ShaderProgram>
        {
        public:
            ShaderProgramResource(const wrl::ComPtr<ID3D12RootSignature>& rootSig, const wrl::ComPtr<ID3DBlob>& vs, const wrl::ComPtr<ID3DBlob>& ps, const rsl::vector<renderer::commands::ConstantLayoutDescription>& constants)
              : BaseResource(&m_shader_program)
                ,m_shader_program({ rootSig, vs, ps, constants})
            {}
            ~ShaderProgramResource() override = default;

        private:
            resources::ShaderProgram m_shader_program;
        };
    } // namespace renderer
} // namespace rex