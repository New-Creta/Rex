#pragma once

#include "rex_directx/system/dx_shader.h"

namespace rex
{
  namespace rhi
  {
    class VertexShader : public DxShader
    {
    public:
      VertexShader(const wrl::ComPtr<ID3DBlob> byteCode);

    private:

    };
  }
}