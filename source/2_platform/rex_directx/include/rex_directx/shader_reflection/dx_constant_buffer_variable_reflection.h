#pragma once

#include <d3d12shader.h>

#include "rex_std/bonus/string.h"
#include "rex_directx/shader_reflection/dx_shader_semantic.h"
#include "rex_renderer_core/shader_reflection/constant_buffer_variable_reflection.h"

namespace rex
{
  namespace gfx
  {
    class DxConstantBufferVariableReflection : public ConstantBufferVariableReflection
    {
    public:
      DxConstantBufferVariableReflection(ID3D12ShaderReflectionVariable* varRefl);

      Semantic semantic() const;

    private:
      ID3D12ShaderReflectionVariable* m_var_relf;
      Semantic m_semantic;
    };

  }
}