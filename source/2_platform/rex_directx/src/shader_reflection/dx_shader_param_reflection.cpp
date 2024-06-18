#include "rex_directx/shader_reflection/dx_shader_param_reflection.h"

#include "rex_engine/diagnostics/assert.h"

namespace rex
{
	namespace gfx
	{
    ShaderParameterReflection::ShaderParameterReflection(const D3D12_SIGNATURE_PARAMETER_DESC& paramDesc)
      : m_semantic_name()
      , m_semantic_index(-1)
      , m_type(ParameterType::Unknown)
      , m_size(0)
    {
      m_semantic_name = paramDesc.SemanticName;
      m_semantic_index = paramDesc.SemanticIndex;

      init_type(paramDesc.ComponentType, paramDesc.Mask, paramDesc.MinPrecision);
      init_size();
    }

    rsl::string_view ShaderParameterReflection::name() const
    {
      return m_semantic_name;
    }
    s32 ShaderParameterReflection::index() const
    {
      return m_semantic_index;
    }
    ParameterType ShaderParameterReflection::type() const
    {
      return m_type;
    }
    s32 ShaderParameterReflection::size() const
    {
      return m_size;
    }

    void ShaderParameterReflection::init_type(D3D_REGISTER_COMPONENT_TYPE componentType, s32 componentMask, D3D_MIN_PRECISION precision)
    {
      switch (componentType)
      {
      case D3D_REGISTER_COMPONENT_UNKNOWN:  REX_ASSERT("Invalid component type");
      case D3D_REGISTER_COMPONENT_UINT32:  m_type = component_mask_to_uint(componentType, precision); break;
      case D3D_REGISTER_COMPONENT_FLOAT32: m_type = component_mask_to_float(componentMask); break;
      }
    }
    void ShaderParameterReflection::init_size()
    {
      m_size = param_type_to_size();
    }

    ParameterType ShaderParameterReflection::component_mask_to_float(s32 componentMask) const
    {
      if (componentMask & 8)
        return ParameterType::Float4;
      if (componentMask & 4)
        return ParameterType::Float3;
      if (componentMask & 2)
        return ParameterType::Float2;
      if (componentMask & 1)
        return ParameterType::Float;

      REX_ASSERT("Invalid component mask");
      return ParameterType::Unknown;
    }
    ParameterType ShaderParameterReflection::component_mask_to_uint(s32 componentMask, D3D_MIN_PRECISION precision) const
    {
      if (componentMask & 3)
      {
        if (precision == D3D_MIN_PRECISION_UINT_16)
        {
          return ParameterType::Uint16_2;
        }
        else if (precision == D3D_MIN_PRECISION_DEFAULT)
        {
          return ParameterType::Uint2;
        }
      }
      if (componentMask & 1)
      {
        return ParameterType::Uint;
      }

      REX_ASSERT("Invalid component mask");
      return ParameterType::Unknown;
    }

    s32 ShaderParameterReflection::param_type_to_size() const
    {
      switch (m_type)
      {
      case ParameterType::Uint:        return 1 * sizeof(uint32);
      case ParameterType::Uint2:       return 2 * sizeof(uint32);
      case ParameterType::Uint16_2:    return 2 * sizeof(uint16);
      case ParameterType::Float:       return 1 * sizeof(float32);
      case ParameterType::Float2:      return 2 * sizeof(float32);
      case ParameterType::Float3:      return 3 * sizeof(float32);
      case ParameterType::Float4:      return 4 * sizeof(float32);
      }

      REX_ASSERT("Invalid parameter type");
      return 0;
    }

	}
}