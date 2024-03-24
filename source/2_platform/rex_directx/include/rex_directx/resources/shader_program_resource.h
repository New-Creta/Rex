#pragma once

#include "rex_directx/utility/directx_util.h"
#include "rex_engine/platform/win/win_com_ptr.h"
#include "rex_renderer_core/resource_management/resource.h"

#include "rex_directx/resources/vertex_shader_resource.h"
#include "rex_directx/resources/pixel_shader_resource.h"

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
                const VertexShaderResource* vertex_shader{};
                const PixelShaderResource* pixel_shader{};

                rsl::unique_array<ConstantLayoutDescription> constants{};
            };
        } // namespace resources

        class ShaderProgramResource : public BaseResource<resources::ShaderProgram>
        {
        public:
            ShaderProgramResource(ResourceHash hash, const wrl::ComPtr<ID3D12RootSignature>& rootSig, const VertexShaderResource* vs, const PixelShaderResource* ps, rsl::unique_array<ConstantLayoutDescription>&& constants)
              : BaseResource(&m_shader_program, hash)
                ,m_shader_program({ rootSig, vs, ps, rsl::move(constants)})
            {}
            ~ShaderProgramResource() override = default;

            ID3D12RootSignature* root_signature()
            {
              return m_shader_program.root_signature.Get();
            }
            D3D12_SHADER_BYTECODE vs()
            {
              return { m_shader_program.vertex_shader->get()->vertex_shader->GetBufferPointer(), m_shader_program.vertex_shader->get()->vertex_shader->GetBufferSize() };
            }
            D3D12_SHADER_BYTECODE ps()
            {
              return { m_shader_program.pixel_shader->get()->pixel_shader->GetBufferPointer(), m_shader_program.pixel_shader->get()->pixel_shader->GetBufferSize() };
            }

        private:
            resources::ShaderProgram m_shader_program;
        };
    } // namespace renderer
} // namespace rex