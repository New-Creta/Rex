#include "rex_directx/resources/dx_vertex_shader.h"

namespace rex
{
  namespace rhi
  {
    DxVertexShader::DxVertexShader(const wrl::ComPtr<ID3DBlob> byteCode)
      : DxShader(byteCode)
    {}
  }
}