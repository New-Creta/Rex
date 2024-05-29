#include "rex_directx/system/dx_shader.h"

namespace rex
{
  namespace rhi
  {
    DxShader::DxShader(const wrl::ComPtr<ID3DBlob>& blob)
      : m_shader_bytecode(blob)
    {}

    D3D12_SHADER_BYTECODE DxShader::dx_bytecode() const
    {
      return { m_shader_bytecode->GetBufferPointer(), m_shader_bytecode->GetBufferSize() };
    }
  }
}