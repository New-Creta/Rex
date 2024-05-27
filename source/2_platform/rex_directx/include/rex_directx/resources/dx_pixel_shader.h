#pragma once

#include "rex_directx/system/dx_shader.h"

namespace rex
{
  namespace rhi
  {
    class PixelShader : public Shader
    {
    public:
      PixelShader(const wrl::ComPtr<ID3DBlob> byteCode);

    private:

    };
  }
}