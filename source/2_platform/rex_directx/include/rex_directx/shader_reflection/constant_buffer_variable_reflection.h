#pragma once

namespace rex
{
  namespace gfx
  {
    class ConstantBufferVariableReflection
    {
    public:
      ConstantBufferVariableReflection(typename ShaderReflectionTypes<DirectXVersion>::ID3DShaderReflectionVariable* var_refl)
        : m_var_relf(var_refl)
      {
        typename ShaderReflectionTypes<DirectXVersion>::D3D_SHADER_VARIABLE_DESC cb_var_desc;
        m_var_relf->GetDesc(&cb_var_desc);

        m_name = cb_var_desc.Name;
        m_offset = cb_var_desc.StartOffset;
        m_size = cb_var_desc.Size;
        m_semantic = var_name_to_semantic(m_name.to_view());
        typename ShaderReflectionTypes<DirectXVersion>::ID3DShaderReflectionType* type = m_var_relf->GetType();
        typename ShaderReflectionTypes<DirectXVersion>::D3D_SHADER_TYPE_DESC type_desc;
        DX_CALL(type->GetDesc(&type_desc));
        m_class_type = type_desc.Class;
        m_type = type_desc.Type;
      }

      const ca::TinyStackString& name() const
      {
        return m_name;
      }
      Semantic semantic() const
      {
        return m_semantic;
      }
      card32 offset() const
      {
        return m_offset;
      }
      card32 size() const
      {
        return m_size;
      }

      ClassType class_type() const
      {
        return m_class_type;
      }
      VarType var_type() const
      {
        return m_type;
      }

    private:
      typename ShaderReflectionTypes<DirectXVersion>::ID3DShaderReflectionVariable* m_var_relf;

      ca::TinyStackString m_name;
      card32 m_offset;
      card32 m_size;
      Semantic m_semantic;
      ClassType m_class_type;
      VarType m_type;
    };

  }
}