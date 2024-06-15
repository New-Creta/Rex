#pragma once

#include "rex_directx/utility/dx_util.h"

namespace rex
{
  namespace gfx
  {
    struct CompileShaderDesc;

    class ShaderCompiler
    {
    public:
      wrl::ComPtr<ID3DBlob> compile_shader(const CompileShaderDesc& desc);
    };
  } // namespace gfx
} // namespace rex