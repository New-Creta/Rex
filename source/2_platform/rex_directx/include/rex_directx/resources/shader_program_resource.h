#pragma once

#include "rex_directx/resources/resource.h"
#include "rex_directx/directx_util.h"
#include "rex_directx/wrl/wrl_types.h"

#include "rex_renderer_core/parameters/constant_layout_description_params.h"

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

                parameters::ConstantLayoutDescription* constants;
                s32 num_constants;
            };
        }

        class ShaderProgramResource : public BaseResource<resources::ShaderProgram>
        {
        public:
            ShaderProgramResource(const wrl::com_ptr<ID3D12RootSignature>& rootSig, const wrl::com_ptr<ID3DBlob>& vs, const wrl::com_ptr<ID3DBlob>& ps, parameters::ConstantLayoutDescription* constants, s32 numConstants)
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