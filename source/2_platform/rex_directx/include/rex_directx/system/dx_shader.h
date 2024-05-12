#pragma once

#include "rex_directx/utility/dx_util.h"

namespace rex
{
  namespace rhi
  {
    class Shader
    {
    public:
      Shader(const wrl::ComPtr<ID3DBlob>& blob);

      D3D12_SHADER_BYTECODE dx_bytecode();

    private:
      wrl::ComPtr<ID3DBlob> m_shader_bytecode;
    };
  }
}