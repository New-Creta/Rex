#pragma once

#include "rex_engine/engine/types.h"
#include "rex_std/bonus/string.h"

#include "rex_directx/shader_reflection/dx_shader_param_type.h"

#include <d3d12shader.h>

namespace rex
{
  namespace gfx
  {
    class ShaderParameterReflection
    {
    public:
      ShaderParameterReflection(const D3D12_SIGNATURE_PARAMETER_DESC& param_desc);

      rsl::string_view name() const;
      s32 index() const;
      ParameterType type() const;
      s32 size() const;

    private:
      void init_type(D3D_REGISTER_COMPONENT_TYPE component_type, s32 componentMask, D3D_MIN_PRECISION precision);
      void init_size();

      ParameterType component_mask_to_float(s32 componentMask) const;
      ParameterType component_mask_to_uint(s32 componentMask, D3D_MIN_PRECISION precision) const;

      s32 param_type_to_size() const;

    private:
      rsl::tiny_stack_string m_semantic_name;
      s32 m_semantic_index;
      ParameterType m_type;
      s32 m_size;
    };

  }
}