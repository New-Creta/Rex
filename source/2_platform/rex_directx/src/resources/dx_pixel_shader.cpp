#include "rex_directx/resources/dx_pixel_shader.h"

namespace rex
{
  namespace rhi
  {
    PixelShader::PixelShader(const wrl::ComPtr<ID3DBlob> byteCode)
      : DxShader(byteCode)
    {}
  }
}