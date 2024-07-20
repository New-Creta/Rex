#include "rex_directx/resources/dx_vertex_shader.h"

namespace rex
{
  namespace gfx
  {
    DxVertexShader::DxVertexShader(const wrl::ComPtr<ID3DBlob> byteCode)
      : DxShader(byteCode, ShaderType::Vertex)
    {}
  }
}