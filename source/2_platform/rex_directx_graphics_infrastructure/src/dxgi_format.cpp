#include "rex_dxgi/dxgi_format.h"
#include "rex_engine/diagnostics/assert.h"

namespace rex
{
  namespace dxgi
  {
    namespace conversions
    {
      DXGI_FORMAT to_DXGI(renderer::Format format)
      {
        switch(format)
        {
          case renderer::Format::UNKNOWN: return DXGI_FORMAT_UNKNOWN;
          case renderer::Format::R32G32B32A32_TYPLESS: return DXGI_FORMAT_R32G32B32A32_TYPELESS;
          case renderer::Format::R32G32B32A32_FLOAT: return DXGI_FORMAT_R32G32B32A32_FLOAT;
          case renderer::Format::R32G32B32A32_UINT: return DXGI_FORMAT_R32G32B32A32_UINT;
          case renderer::Format::R32G32B32A32_SINT: return DXGI_FORMAT_R32G32B32A32_SINT;
          case renderer::Format::R32G32B32_TYPELESS: return DXGI_FORMAT_R32G32B32_TYPELESS;
          case renderer::Format::R32G32B32_FLOAT: return DXGI_FORMAT_R32G32B32_FLOAT;
          case renderer::Format::R32G32B32_UINT: return DXGI_FORMAT_R32G32B32_UINT;
          case renderer::Format::R32G32B32_SINT: return DXGI_FORMAT_R32G32B32_SINT;
          case renderer::Format::R16G16B16A16_TYPELESS: return DXGI_FORMAT_R16G16B16A16_TYPELESS;
          case renderer::Format::R16G16B16A16_FLOAT: return DXGI_FORMAT_R16G16B16A16_FLOAT;
          case renderer::Format::R16G16B16A16_UNORM: return DXGI_FORMAT_R16G16B16A16_UNORM;
          case renderer::Format::R16G16B16A16_UINT: return DXGI_FORMAT_R16G16B16A16_UINT;
          case renderer::Format::R16G16B16A16_SNORM: return DXGI_FORMAT_R16G16B16A16_SNORM;
          case renderer::Format::R16G16B16A16_SINT: return DXGI_FORMAT_R16G16B16A16_SINT;
          case renderer::Format::R32G32_TYPELESS: return DXGI_FORMAT_R32G32_TYPELESS;
          case renderer::Format::R32G32_FLOAT: return DXGI_FORMAT_R32G32_FLOAT;
          case renderer::Format::R32G32_UINT: return DXGI_FORMAT_R32G32_UINT;
          case renderer::Format::R32G32_SINT: return DXGI_FORMAT_R32G32_SINT;
          case renderer::Format::R32G8X24_TYPELESS: return DXGI_FORMAT_R32G8X24_TYPELESS;
          case renderer::Format::D32_FLOAT_S8X24_UINT: return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
          case renderer::Format::R32_FLOAT_X8X24_TYPELESS: return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
          case renderer::Format::X32_TYPELESS_G8X24_UINT: return DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;
          case renderer::Format::R10G10B10A2_TYPELESS: return DXGI_FORMAT_R10G10B10A2_TYPELESS;
          case renderer::Format::R10G10B10A2_UNORM: return DXGI_FORMAT_R10G10B10A2_UNORM;
          case renderer::Format::R10G10B10A2_UINT: return DXGI_FORMAT_R10G10B10A2_UINT;
          case renderer::Format::R11G11B10_FLOAT: return DXGI_FORMAT_R11G11B10_FLOAT;
          case renderer::Format::R8G8B8A8_TYPELESS: return DXGI_FORMAT_R8G8B8A8_TYPELESS;
          case renderer::Format::R8G8B8A8_UNORM: return DXGI_FORMAT_R8G8B8A8_UNORM;
          case renderer::Format::R8G8B8A8_UNORM_SRGB: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
          case renderer::Format::R8G8B8A8_UINT: return DXGI_FORMAT_R8G8B8A8_UINT;
          case renderer::Format::R8G8B8A8_SNORM: return DXGI_FORMAT_R8G8B8A8_SNORM;
          case renderer::Format::R8G8B8A8_SINT: return DXGI_FORMAT_R8G8B8A8_SINT;
          case renderer::Format::R16G16_TYPELESS: return DXGI_FORMAT_R16G16_TYPELESS;
          case renderer::Format::R16G16_FLOAT: return DXGI_FORMAT_R16G16_FLOAT;
          case renderer::Format::R16G16_UNORM: return DXGI_FORMAT_R16G16_UNORM;
          case renderer::Format::R16G16_UINT: return DXGI_FORMAT_R16G16_UINT;
          case renderer::Format::R16G16_SNORM: return DXGI_FORMAT_R16G16_SNORM;
          case renderer::Format::R16G16_SINT: return DXGI_FORMAT_R16G16_SINT;
          case renderer::Format::R32_TYPELESS: return DXGI_FORMAT_R32_TYPELESS;
          case renderer::Format::D32_FLOAT: return DXGI_FORMAT_D32_FLOAT;
          case renderer::Format::R32_FLOAT: return DXGI_FORMAT_R32_FLOAT;
          case renderer::Format::R32_UINT: return DXGI_FORMAT_R32_UINT;
          case renderer::Format::R32_SINT: return DXGI_FORMAT_R32_SINT;
          case renderer::Format::R24G8_TYPELESS: return DXGI_FORMAT_R24G8_TYPELESS;
          case renderer::Format::D24_UNORM_S8_UINT: return DXGI_FORMAT_D24_UNORM_S8_UINT;
          case renderer::Format::R24_UNORM_X8_TYPELESS: return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
          case renderer::Format::X24_TYPELESS_G8_UINT: return DXGI_FORMAT_X24_TYPELESS_G8_UINT;
          case renderer::Format::R8G8_TYPELESS: return DXGI_FORMAT_R8G8_TYPELESS;
          case renderer::Format::R8G8_UNORM: return DXGI_FORMAT_R8G8_UNORM;
          case renderer::Format::R8G8_UINT: return DXGI_FORMAT_R8G8_UINT;
          case renderer::Format::R8G8_SNORM: return DXGI_FORMAT_R8G8_SNORM;
          case renderer::Format::R8G8_SINT: return DXGI_FORMAT_R8G8_SINT;
          case renderer::Format::R16_TYPELESS: return DXGI_FORMAT_R16_TYPELESS;
          case renderer::Format::R16_FLOAT: return DXGI_FORMAT_R16_FLOAT;
          case renderer::Format::D16_UNORM: return DXGI_FORMAT_D16_UNORM;
          case renderer::Format::R16_UNORM: return DXGI_FORMAT_R16_UNORM;
          case renderer::Format::R16_UINT: return DXGI_FORMAT_R16_UINT;
          case renderer::Format::R16_SNORM: return DXGI_FORMAT_R16_SNORM;
          case renderer::Format::R16_SINT: return DXGI_FORMAT_R16_SINT;
          case renderer::Format::R8_TYPELESS: return DXGI_FORMAT_R8_TYPELESS;
          case renderer::Format::R8_UNORM: return DXGI_FORMAT_R8_UNORM;
          case renderer::Format::R8_UINT: return DXGI_FORMAT_R8_UINT;
          case renderer::Format::R8_SNORM: return DXGI_FORMAT_R8_SNORM;
          case renderer::Format::R8_SINT: return DXGI_FORMAT_R8_SINT;
          case renderer::Format::A8_UNORM: return DXGI_FORMAT_A8_UNORM;
          case renderer::Format::R1_UNORM: return DXGI_FORMAT_R1_UNORM;
          case renderer::Format::R9G9B9E5_SHAREDEXP: return DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
          case renderer::Format::R8G8_B8G8_UNORM: return DXGI_FORMAT_R8G8_B8G8_UNORM;
          case renderer::Format::G8R8_G8B8_UNORM: return DXGI_FORMAT_G8R8_G8B8_UNORM;
          case renderer::Format::BC1_TYPELESS: return DXGI_FORMAT_BC1_TYPELESS;
          case renderer::Format::BC1_UNORM: return DXGI_FORMAT_BC1_UNORM;
          case renderer::Format::BC1_UNORM_SRGB: return DXGI_FORMAT_BC1_UNORM_SRGB;
          case renderer::Format::BC2_TYPELESS: return DXGI_FORMAT_BC2_TYPELESS;
          case renderer::Format::BC2_UNORM: return DXGI_FORMAT_BC2_UNORM;
          case renderer::Format::BC2_UNORM_SRGB: return DXGI_FORMAT_BC2_UNORM_SRGB;
          case renderer::Format::BC3_TYPELESS: return DXGI_FORMAT_BC3_TYPELESS;
          case renderer::Format::BC3_UNORM: return DXGI_FORMAT_BC3_UNORM;
          case renderer::Format::BC3_UNORM_SRGB: return DXGI_FORMAT_BC3_UNORM_SRGB;
          case renderer::Format::BC4_TYPELESS: return DXGI_FORMAT_BC4_TYPELESS;
          case renderer::Format::BC4_UNORM: return DXGI_FORMAT_BC4_UNORM;
          case renderer::Format::BC4_SNORM: return DXGI_FORMAT_BC4_SNORM;
          case renderer::Format::BC5_TYPELESS: return DXGI_FORMAT_BC5_TYPELESS;
          case renderer::Format::BC5_UNORM: return DXGI_FORMAT_BC5_UNORM;
          case renderer::Format::BC5_SNORM: return DXGI_FORMAT_BC5_SNORM;
          case renderer::Format::B5G6R5_UNORM: return DXGI_FORMAT_B5G6R5_UNORM;
          case renderer::Format::B5G5R5A1_UNORM: return DXGI_FORMAT_B5G5R5A1_UNORM;
          case renderer::Format::B8G8R8A8_UNORM: return DXGI_FORMAT_B8G8R8A8_UNORM;
          case renderer::Format::B8G8R8X8_UNORM: return DXGI_FORMAT_B8G8R8X8_UNORM;
          case renderer::Format::R10G10B10_XR_BIAS_A2_UNORM: return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;
          case renderer::Format::B8G8R8A8_TYPELESS: return DXGI_FORMAT_B8G8R8A8_TYPELESS;
          case renderer::Format::B8G8R8A8_UNORM_SRGB: return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
          case renderer::Format::B8G8R8X8_TYPELESS: return DXGI_FORMAT_B8G8R8X8_TYPELESS;
          case renderer::Format::B8G8R8X8_UNORM_SRGB: return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
          case renderer::Format::B4G4R4A4_UNORM: return DXGI_FORMAT_B4G4R4A4_UNORM;
        }

        REX_ASSERT("Unsupported format");
        return DXGI_FORMAT_UNKNOWN;
      }
    } // namespace conversions
  }   // namespace renderer
} // namespace rex