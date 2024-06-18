#pragma once

#include <d3d12shader.h>

#include "rex_std/string_view.h"
#include "rex_engine/engine/types.h"

#include "rex_directx/shader_reflection/dx_constant_buffer_variable_reflection.h"

namespace rex
{
  namespace gfx
  {
    class ConstantBufferReflection
    {
    public:
      ConstantBufferReflection(ID3D12ShaderReflectionConstantBuffer* cb_refl, s32 shader_register);

      rsl::string_view name() const;
      s32 size() const;
      s32 shader_register() const;
      rsl::vector<ConstantBufferVariableReflection> variables() const;

    private:
      void reflect_variables(s32 numVariables);

    private:
      ID3D12ShaderReflectionConstantBuffer* m_cb_refl;

      rsl::tiny_stack_string m_name;
      s32 m_size;
      s32 m_shader_register;
      rsl::vector<ConstantBufferVariableReflection> m_variables_reflection;
    };

  }
}