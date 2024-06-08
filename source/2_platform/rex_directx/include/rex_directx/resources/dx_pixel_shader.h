#pragma once

#include "rex_directx/system/dx_shader.h"

namespace rex
{
  namespace rhi
  {
    class DxPixelShader : public DxShader
    {
    public:
      DxPixelShader(const wrl::ComPtr<ID3DBlob> byteCode);

    private:

    };
  }
}