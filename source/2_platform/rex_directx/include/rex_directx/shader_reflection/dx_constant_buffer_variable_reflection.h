#pragma once

#include <d3d12shader.h>

#include "rex_std/bonus/string.h"
#include "rex_directx/shader_reflection/dx_shader_semantic.h"

namespace rex
{
  namespace gfx
  {
    class ConstantBufferVariableReflection
    {
    public:
      ConstantBufferVariableReflection(ID3D12ShaderReflectionVariable* varRefl);

      rsl::string_view name() const;
      Semantic semantic() const;
      card32 offset() const;
      card32 size() const;

      ClassType class_type() const;
      VarType var_type() const;

    private:
      ID3D12ShaderReflectionVariable* m_var_relf;

      rsl::tiny_stack_string m_name;
      card32 m_offset;
      card32 m_size;
      Semantic m_semantic;
      ClassType m_class_type;
      VarType m_type;
    };

  }
}