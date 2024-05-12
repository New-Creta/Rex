#include "rex_directx/system/dx_shader.h"

namespace rex
{
  namespace rhi
  {
    Shader::Shader(const wrl::ComPtr<ID3DBlob>& blob)
      : m_shader_bytecode(blob)
    {}

    D3D12_SHADER_BYTECODE Shader::dx_bytecode()
    {
      return { m_shader_bytecode->GetBufferPointer(), m_shader_bytecode->GetBufferSize() };
    }
  }
}