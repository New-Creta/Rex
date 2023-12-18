#pragma once

#include "rex_renderer_core/resource.h"
#include "rex_directx/directx_util.h"
#include "rex_directx/wrl/wrl_types.h"

#include "rex_renderer_core/commands/create_constant_layout_description_cmd.h"

namespace rex
{
    namespace renderer
    {
        namespace resources
        {
            struct ShaderProgram
            {
                wrl::com_ptr<ID3D12RootSignature> root_signature;
                wrl::com_ptr<ID3DBlob> vertex_shader;
                wrl::com_ptr<ID3DBlob> pixel_shader;

                const commands::ConstantLayoutDescription* constants;
                s32 num_constants;
            };
        }

        class ShaderProgramResource : public BaseResource<resources::ShaderProgram>
        {
        public:
            RESOURCE_CLASS_TYPE(ShaderProgramResource);

            ShaderProgramResource(const wrl::com_ptr<ID3D12RootSignature>& rootSig, const wrl::com_ptr<ID3DBlob>& vs, const wrl::com_ptr<ID3DBlob>& ps, const commands::ConstantLayoutDescription* constants, s32 numConstants)
                :m_shader_program({ rootSig, vs, ps, constants, numConstants })
            {}
            ~ShaderProgramResource() override = default;

            resources::ShaderProgram* get()
            {
                return &m_shader_program;
            }
            const resources::ShaderProgram* get() const
            {
                return &m_shader_program;
            }

        private:
            resources::ShaderProgram m_shader_program;
        };
    }
}