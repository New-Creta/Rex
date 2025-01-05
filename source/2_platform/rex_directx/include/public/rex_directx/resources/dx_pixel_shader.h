#pragma once

#include "rex_directx/system/dx_shader.h"

namespace rex
{
  namespace gfx
  {
    class DxPixelShader : public DxShader
    {
    public:
      DxPixelShader(const wrl::ComPtr<ID3DBlob> byteCode);

    private:

    };
  }
}