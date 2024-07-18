#include "rex_directx/shader_reflection/dx_shader_param_reflection.h"

#include "rex_engine/diagnostics/assert.h"

#include "rex_directx/utility/dx_util.h"

namespace rex
{
	namespace gfx
	{
    DxShaderParameterReflection::DxShaderParameterReflection(const D3D12_SIGNATURE_PARAMETER_DESC& paramDesc)
      : m_semantic_name()
      , m_semantic_index(-1)
      , m_type()
      , m_size(0)
    {
      m_semantic_name = paramDesc.SemanticName;
      m_semantic_index = paramDesc.SemanticIndex;

      init_type(paramDesc.ComponentType, paramDesc.Mask, paramDesc.MinPrecision);
      init_size();
    }

    rsl::string_view DxShaderParameterReflection::name() const
    {
      return m_semantic_name;
    }
    s32 DxShaderParameterReflection::index() const
    {
      return m_semantic_index;
    }
    ShaderParameterType DxShaderParameterReflection::type() const
    {
      return m_type;
    }
    s32 DxShaderParameterReflection::size() const
    {
      return m_size;
    }

    void DxShaderParameterReflection::init_type(D3D_REGISTER_COMPONENT_TYPE componentType, s32 componentMask, D3D_MIN_PRECISION precision)
    {
      switch (componentType)
      {
      case D3D_REGISTER_COMPONENT_UNKNOWN:  REX_ASSERT("Invalid component type");
      case D3D_REGISTER_COMPONENT_UINT32:  m_type = component_mask_to_uint(componentType, precision); break;
      case D3D_REGISTER_COMPONENT_FLOAT32: m_type = component_mask_to_float(componentMask); break;
      }
    }
    void DxShaderParameterReflection::init_size()
    {
      m_size = format_byte_size(m_type);
    }

    ShaderParameterType DxShaderParameterReflection::component_mask_to_float(s32 componentMask) const
    {
      if (componentMask & 8)
        return ShaderParameterType::Float4;
      if (componentMask & 4)
        return ShaderParameterType::Float3;
      if (componentMask & 2)
        return ShaderParameterType::Float2;
      if (componentMask & 1)
        return ShaderParameterType::Float;

      REX_ASSERT("Invalid component mask");
      return ShaderParameterType::Unknown;
    }
    ShaderParameterType DxShaderParameterReflection::component_mask_to_uint(s32 componentMask, D3D_MIN_PRECISION precision) const
    {
      if (componentMask & 3)
      {
        if (precision == D3D_MIN_PRECISION_UINT_16)
        {
          return ShaderParameterType::Ushort2;
        }
        else if (precision == D3D_MIN_PRECISION_DEFAULT)
        {
          return ShaderParameterType::Uint2;
        }
      }
      if (componentMask & 1)
      {
        return ShaderParameterType::Uint;
      }

      REX_ASSERT("Invalid component mask");
      return ShaderParameterType::Unknown;
    }
	}
}