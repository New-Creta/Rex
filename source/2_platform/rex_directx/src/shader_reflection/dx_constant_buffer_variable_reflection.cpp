#include "rex_directx/shader_reflection/dx_constant_buffer_variable_reflection.h"

#include "rex_directx/diagnostics/dx_call.h"
#include "rex_directx/utility/dx_util.h"

namespace rex
{
	namespace gfx
	{
    CBufferVarReflDesc create_desc(ID3D12ShaderReflectionVariable* varRefl)
    {
      CBufferVarReflDesc desc{};
      D3D12_SHADER_VARIABLE_DESC cb_var_desc;
      varRefl->GetDesc(&cb_var_desc);

      desc.name = cb_var_desc.Name;
      desc.offset = cb_var_desc.StartOffset;
      desc.size = cb_var_desc.Size;
      ID3D12ShaderReflectionType* type = varRefl->GetType();
      D3D12_SHADER_TYPE_DESC type_desc;
      DX_CALL(type->GetDesc(&type_desc));
      desc.class_type = d3d::from_dx12(type_desc.Class);
      desc.var_type = d3d::from_dx12(type_desc.Type);

      return desc;
    }

    DxConstantBufferVariableReflection::DxConstantBufferVariableReflection(ID3D12ShaderReflectionVariable* varRefl)
      : ConstantBufferVariableReflection(create_desc(varRefl))
      , m_var_relf(varRefl)
    {
      D3D12_SHADER_VARIABLE_DESC cb_var_desc;
      m_var_relf->GetDesc(&cb_var_desc);
      m_semantic = var_name_to_semantic(name);
    }

    Semantic DxConstantBufferVariableReflection::semantic() const
    {
      return m_semantic;
    }
	}
}