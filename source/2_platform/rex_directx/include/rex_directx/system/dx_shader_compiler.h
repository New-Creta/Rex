#pragma once

#include "rex_directx/utility/dx_util.h"

namespace rex
{
  namespace rhi
  {
    struct CompileShaderDesc;

    class ShaderCompiler
    {
    public:
      wrl::ComPtr<ID3DBlob> compile_shader(const CompileShaderDesc& desc);
    };
  }
}