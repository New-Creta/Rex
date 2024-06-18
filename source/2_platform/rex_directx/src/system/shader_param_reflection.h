#pragma once

namespace rex
{
  namespace gfx
  {
    template <card32 DirectXVersion>
    class ShaderParameterReflection
    {
    public:
      ShaderParameterReflection(const typename ShaderReflectionTypes<DirectXVersion>::D3D_SIGNATURE_PARAMETER_DESC& param_desc)
        : m_semantic_name()
        , m_semantic_index(-1)
        , m_type(dx::ParameterType::Unknown)
        , m_size(0)
      {
        m_semantic_name = param_desc.SemanticName;
        m_semantic_index = param_desc.SemanticIndex;

        init_type(param_desc.ComponentType, param_desc.Mask, param_desc.MinPrecision);
        init_size();
      }

      const ca::TinyStackString& name() const
      {
        return m_semantic_name;
      }
      card32 index() const
      {
        return m_semantic_index;
      }
      dx::ParameterType type() const
      {
        return m_type;
      }
      card32 size() const
      {
        return m_size;
      }

    private:
      void init_type(D3D_REGISTER_COMPONENT_TYPE component_type, card32 component_mask, D3D_MIN_PRECISION precision)
      {
        switch (component_type)
        {
        case D3D_REGISTER_COMPONENT_UNKNOWN:  CA_ASSERT("Invalid component type");
        case D3D_REGISTER_COMPONENT_UINT32:  m_type = component_mask_to_uint(component_type, precision); break;
        case D3D_REGISTER_COMPONENT_FLOAT32: m_type = component_mask_to_float(component_mask); break;
        }
      }
      void init_size()
      {
        m_size = param_type_to_size();
      }

      dx::ParameterType component_mask_to_float(card32 component_mask) const
      {
        if (component_mask & 8)
          return dx::ParameterType::Float4;
        if (component_mask & 4)
          return dx::ParameterType::Float3;
        if (component_mask & 2)
          return dx::ParameterType::Float2;
        if (component_mask & 1)
          return dx::ParameterType::Float;

        CA_ASSERT("Invalid component mask");
        return dx::ParameterType::Unknown;
      }
      dx::ParameterType component_mask_to_uint(card32 component_mask, D3D_MIN_PRECISION precision) const
      {
        if (component_mask & 3)
        {
          if (precision == D3D_MIN_PRECISION_UINT_16)
          {
            return dx::ParameterType::Uint16_2;
          }
          else if (precision == D3D_MIN_PRECISION_DEFAULT)
          {
            return dx::ParameterType::Uint2;
          }
        }
        if (component_mask & 1)
        {
          return dx::ParameterType::Uint;
        }

        CA_ASSERT("Invalid component mask");
        return dx::ParameterType::Unknown;
      }

      card32 param_type_to_size() const
      {
        switch (m_type)
        {
        case spartan::dx::ParameterType::Uint:        return 1 * sizeof(uint32);
        case spartan::dx::ParameterType::Uint2:       return 2 * sizeof(uint32);
        case spartan::dx::ParameterType::Uint16_2:    return 2 * sizeof(uint16);
        case spartan::dx::ParameterType::Float:       return 1 * sizeof(float32);
        case spartan::dx::ParameterType::Float2:      return 2 * sizeof(float32);
        case spartan::dx::ParameterType::Float3:      return 3 * sizeof(float32);
        case spartan::dx::ParameterType::Float4:      return 4 * sizeof(float32);
        }

        CA_ASSERT("Invalid parameter type");
        return 0;
      }

    private:
      ca::TinyStackString m_semantic_name;
      card32 m_semantic_index;
      dx::ParameterType m_type;
      card32 m_size;
    };

  }
}