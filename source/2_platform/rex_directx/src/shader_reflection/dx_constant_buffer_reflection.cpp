#include "rex_directx/shader_reflection/dx_constant_buffer_reflection.h"

namespace rex
{
	namespace gfx
	{
    DxConstantBufferReflection::DxConstantBufferReflection(ID3D12ShaderReflectionConstantBuffer* cbRefl, s32 shaderRegister)
      : m_cb_refl(cbRefl)
      , m_shader_register(shaderRegister)
    {
      D3D12_SHADER_BUFFER_DESC cb_desc;
      m_cb_refl->GetDesc(&cb_desc);

      m_name = cb_desc.Name;
      m_size = cb_desc.Size;

      reflect_variables(cb_desc.Variables);
    }

    rsl::string_view DxConstantBufferReflection::name() const
    {
      return m_name;
    }
    card32 DxConstantBufferReflection::size() const
    {
      return m_size;
    }
    card32 DxConstantBufferReflection::shader_register() const
    {
      return m_shader_register;
    }
    rsl::vector<ConstantBufferVariableReflection> DxConstantBufferReflection::variables() const
    {
      return m_variables_reflection;
    }

    void DxConstantBufferReflection::reflect_variables(s32 numVariables)
    {
      m_variables_reflection.reserve(numVariables);

      for (card32 var_idx = 0; var_idx < numVariables; ++var_idx)
      {
        ID3D12ShaderReflectionVariable* cb_var = m_cb_refl->GetVariableByIndex(var_idx);
        //m_variables_reflection.emplace_back(cb_var);
      }
    }
	}
}