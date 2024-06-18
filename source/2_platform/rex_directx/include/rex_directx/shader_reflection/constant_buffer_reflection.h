#pragma once

namespace rex
{
  namespace gfx
  {
    template <card32 DirectXVersion>
    class ConstantBufferReflection
    {
    public:
      ConstantBufferReflection(typename ShaderReflectionTypes<DirectXVersion>::ID3DShaderReflectionConstantBuffer* cb_refl, card32 shader_register)
        : m_cb_refl(cb_refl)
        , m_shader_register(shader_register)
      {
        typename ShaderReflectionTypes<DirectXVersion>::D3D_SHADER_BUFFER_DESC cb_desc;
        m_cb_refl->GetDesc(&cb_desc);

        m_name = cb_desc.Name;
        m_size = cb_desc.Size;

        reflect_variables(cb_desc.Variables);
      }

      const ca::TinyStackString& name() const
      {
        return m_name;
      }
      card32 size() const
      {
        return m_size;
      }
      card32 shader_register() const
      {
        return m_shader_register;
      }
      ca::ConstVectorView<ConstantBufferVariableReflection<DirectXVersion>> variables() const
      {
        return ca::ConstVectorView<ConstantBufferVariableReflection<DirectXVersion>>(m_variables_reflection);
      }

    private:
      void reflect_variables(card32 num_variables)
      {
        m_variables_reflection.reserve(num_variables);

        for (card32 var_idx = 0; var_idx < num_variables; ++var_idx)
        {
          typename ShaderReflectionTypes<DirectXVersion>::ID3DShaderReflectionVariable* cb_var = m_cb_refl->GetVariableByIndex(var_idx);
          m_variables_reflection.emplace_back(cb_var);
        }
      }

    private:
      typename ShaderReflectionTypes<DirectXVersion>::ID3DShaderReflectionConstantBuffer* m_cb_refl;

      ca::TinyStackString m_name;
      card32 m_size;
      card32 m_shader_register;
      ca::vector<ConstantBufferVariableReflection<DirectXVersion>> m_variables_reflection;
    };

  }
}