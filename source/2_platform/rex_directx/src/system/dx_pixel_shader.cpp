#include "rex_directx/system/dx_pixel_shader.h"

namespace rex
{
  namespace rhi
  {
    PixelShader::PixelShader(const wrl::ComPtr<ID3DBlob> byteCode)
      : Shader(byteCode)
    {}
  }
}