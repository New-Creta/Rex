#pragma once

#include "rex_directx/utility/dx_util.h"

#include "rex_renderer_core/resources/shader.h"

namespace rex
{
  namespace rhi
  {
    class DxShader : public Shader
    {
    public:
      DxShader(const wrl::ComPtr<ID3DBlob>& blob);

      D3D12_SHADER_BYTECODE dx_bytecode() const;

    private:
      wrl::ComPtr<ID3DBlob> m_shader_bytecode;
    };
  }
}