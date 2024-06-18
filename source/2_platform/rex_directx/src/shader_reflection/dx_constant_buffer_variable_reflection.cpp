#include "rex_directx/shader_reflection/dx_constant_buffer_variable_reflection.h"

#include "rex_directx/diagnostics/dx_call.h"

namespace rex
{
	namespace gfx
	{
    ConstantBufferVariableReflection::ConstantBufferVariableReflection(ID3D12ShaderReflectionVariable* varRefl)
      : m_var_relf(varRefl)
    {
      D3D12_SHADER_VARIABLE_DESC cb_var_desc;
      m_var_relf->GetDesc(&cb_var_desc);

      m_name = cb_var_desc.Name;
      m_offset = cb_var_desc.StartOffset;
      m_size = cb_var_desc.Size;
      m_semantic = var_name_to_semantic(m_name.to_view());
      ID3D12ShaderReflectionType* type = m_var_relf->GetType();
      D3D12_SHADER_TYPE_DESC type_desc;
      DX_CALL(type->GetDesc(&type_desc));
      m_class_type = type_desc.Class;
      m_type = type_desc.Type;
    }

    rsl::string_view ConstantBufferVariableReflection::name() const
    {
      return m_name;
    }
    Semantic ConstantBufferVariableReflection::semantic() const
    {
      return m_semantic;
    }
    card32 ConstantBufferVariableReflection::offset() const
    {
      return m_offset;
    }
    card32 ConstantBufferVariableReflection::size() const
    {
      return m_size;
    }

    D3D_SHADER_VARIABLE_CLASS ConstantBufferVariableReflection::class_type() const
    {
      return m_class_type;
    }
    D3D_SHADER_VARIABLE_TYPE ConstantBufferVariableReflection::var_type() const
    {
      return m_type;
    }

	}
}