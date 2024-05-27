#include "rex_directx/system/dx_vertex_shader.h"

namespace rex
{
  namespace rhi
  {
    VertexShader::VertexShader(const wrl::ComPtr<ID3DBlob> byteCode)
      : Shader(byteCode)
    {}
  }
}