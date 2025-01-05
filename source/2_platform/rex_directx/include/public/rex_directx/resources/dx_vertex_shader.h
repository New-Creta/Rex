#pragma once

#include "rex_directx/system/dx_shader.h"

namespace rex
{
  namespace gfx
  {
    class DxVertexShader : public DxShader
    {
    public:
      DxVertexShader(const wrl::ComPtr<ID3DBlob> byteCode);

    private:

    };
  }
}