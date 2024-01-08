#pragma once

#include "rex_renderer_core/resource.h"
#include "rex_directx/directx_util.h"
#include "rex_engine/win/win_com_ptr.h"

#include "rex_renderer_core/commands/create_constant_layout_description_cmd.h"

namespace rex
{
    namespace renderer
    {
        namespace resources
        {
            struct ShaderProgram
            {
                wrl::ComPtr<ID3D12RootSignature> root_signature;
                wrl::ComPtr<ID3DBlob> vertex_shader;
                wrl::ComPtr<ID3DBlob> pixel_shader;

                const commands::ConstantLayoutDescription* constants;
                s32 num_constants;
            };
        }

        class ShaderProgramResource : public BaseResource<resources::ShaderProgram>
        {
        public:
            RESOURCE_CLASS_TYPE(ShaderProgramResource);

            ShaderProgramResource(const wrl::ComPtr<ID3D12RootSignature>& rootSig, const wrl::ComPtr<ID3DBlob>& vs, const wrl::ComPtr<ID3DBlob>& ps, const commands::ConstantLayoutDescription* constants, s32 numConstants)
              : BaseResource(&m_shader_program)
                ,m_shader_program({ rootSig, vs, ps, constants, numConstants })
            {}
            ~ShaderProgramResource() override = default;

        private:
            resources::ShaderProgram m_shader_program;
        };
    }
}