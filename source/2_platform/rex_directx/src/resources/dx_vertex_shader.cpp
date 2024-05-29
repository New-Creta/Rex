#include "rex_directx/resources/dx_vertex_shader.h"

namespace rex
{
  namespace rhi
  {
    VertexShader::VertexShader(const wrl::ComPtr<ID3DBlob> byteCode)
      : DxShader(byteCode)
    {}
  }
}