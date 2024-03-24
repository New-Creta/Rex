#include "rex_directx/utility/directx_util.h"
#include "rex_directx/d3dx12.h"
#include "rex_directx/diagnostics/log.h"

#include "rex_renderer_core/rendering/index_buffer_format.h"

#include <d3dcompiler.h>

namespace rex
{
  namespace renderer
  {
    namespace directx
    {

    } // namespace directx
  }   // namespace renderer

  namespace d3d
  {
    // This isn't great as there isn't a way to pass the memory accross
// from our blob to a d3d blob, we need to copy it over.
// This means that memory will be duplicated.
    wrl::ComPtr<ID3DBlob> create_blob(const memory::Blob& blob)
    {
      wrl::ComPtr<ID3DBlob> d3d_blob;
      D3DCreateBlob(blob.size(), d3d_blob.GetAddressOf());
      memcpy(d3d_blob->GetBufferPointer(), (void*)blob.data(), blob.size());
      return d3d_blob;
    }
    //-------------------------------------------------------------------------
    D3D12_FILL_MODE to_d3d12_fill_mode(renderer::FillMode mode)
    {
      switch (mode)
      {
      case renderer::FillMode::SOLID: return D3D12_FILL_MODE_SOLID;
      case renderer::FillMode::WIREFRAME: return D3D12_FILL_MODE_WIREFRAME;
      default: break;
      }

      REX_ASSERT("Unsupported fill mode given");
      return D3D12_FILL_MODE_SOLID;
    }
    //-------------------------------------------------------------------------
    D3D12_CULL_MODE to_d3d12_cull_mode(renderer::CullMode mode)
    {
      switch (mode)
      {
      case renderer::CullMode::None: return D3D12_CULL_MODE_NONE;
      case renderer::CullMode::FRONT: return D3D12_CULL_MODE_FRONT;
      case renderer::CullMode::BACK: return D3D12_CULL_MODE_BACK;
      default: break;
      }

      REX_ASSERT("Unsupported cull mode given");
      return D3D12_CULL_MODE_NONE;
    }
    //-------------------------------------------------------------------------
    DXGI_FORMAT to_d3d12_vertex_format(renderer::VertexBufferFormat format)
    {
      switch (format)
      {
      case renderer::VertexBufferFormat::FLOAT1: return DXGI_FORMAT_R32_FLOAT;
      case renderer::VertexBufferFormat::FLOAT2: return DXGI_FORMAT_R32G32_FLOAT;
      case renderer::VertexBufferFormat::FLOAT3: return DXGI_FORMAT_R32G32B32_FLOAT;
      case renderer::VertexBufferFormat::FLOAT4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
      case renderer::VertexBufferFormat::UNORM1: return DXGI_FORMAT_R8_UNORM;
      case renderer::VertexBufferFormat::UNORM2: return DXGI_FORMAT_R8G8_UNORM;
      case renderer::VertexBufferFormat::UNORM4: return DXGI_FORMAT_R8G8B8A8_UNORM;
      default: break;
      }
      REX_ASSERT("Unsupported vertex buffer format given");
      return DXGI_FORMAT_UNKNOWN;
    }
    //-------------------------------------------------------------------------
    DXGI_FORMAT to_d3d12_index_format(renderer::IndexBufferFormat format)
    {
      switch (format)
      {
      case renderer::IndexBufferFormat::Uint16: return DXGI_FORMAT_R16_UINT;
      case renderer::IndexBufferFormat::Uint32: return DXGI_FORMAT_R32_UINT;
      default: break;
      }
      REX_ASSERT("Unsupported index buffer format given");
      return DXGI_FORMAT_UNKNOWN;
    }
    //-------------------------------------------------------------------------
    DXGI_FORMAT to_dxd12_texture_format(renderer::TextureFormat format)
    {
      switch (format)
      {
      case renderer::TextureFormat::Unorm4Srgb: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
      default: break;
      }
      REX_ASSERT("Unsupported vertex buffer format given");
      return DXGI_FORMAT_UNKNOWN;
    }

    //-------------------------------------------------------------------------
    D3D12_PRIMITIVE_TOPOLOGY to_d3d12_topology(renderer::PrimitiveTopology topology)
    {
      switch (topology)
      {
      case renderer::PrimitiveTopology::LINELIST: return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
      case renderer::PrimitiveTopology::LINESTRIP: return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
      case renderer::PrimitiveTopology::POINTLIST: return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
      case renderer::PrimitiveTopology::TRIANGLELIST: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
      case renderer::PrimitiveTopology::TRIANGLESTRIP: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
      default: break;
      }
      REX_ASSERT("Unsupported primitive topology given");
      return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    }
    //-------------------------------------------------------------------------
    D3D12_INPUT_CLASSIFICATION to_d3d12_input_layout_classification(renderer::InputLayoutClassification classification)
    {
      switch (classification)
      {
      case renderer::InputLayoutClassification::PerVertexData: return D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
      case renderer::InputLayoutClassification::PerInstanceData: return D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
      default: break;
      }

      REX_ASSERT("Unsupported input layout classification given");
      return D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
    }
    s32 format_byte_size(DXGI_FORMAT format)
    {
      switch (format)
      {
      case DXGI_FORMAT_UNKNOWN:
        return 0;

      case DXGI_FORMAT_R32G32B32A32_TYPELESS:
      case DXGI_FORMAT_R32G32B32A32_FLOAT:
      case DXGI_FORMAT_R32G32B32A32_UINT:
      case DXGI_FORMAT_R32G32B32A32_SINT:
        return 16;

      case DXGI_FORMAT_R32G32B32_TYPELESS:
      case DXGI_FORMAT_R32G32B32_FLOAT:
      case DXGI_FORMAT_R32G32B32_UINT:
      case DXGI_FORMAT_R32G32B32_SINT:
        return 12;

      case DXGI_FORMAT_R16G16_TYPELESS:
      case DXGI_FORMAT_R16G16_FLOAT:
      case DXGI_FORMAT_R16G16_UNORM:
      case DXGI_FORMAT_R16G16_UINT:
      case DXGI_FORMAT_R16G16_SNORM:
      case DXGI_FORMAT_R16G16_SINT:
        return 4;

      case DXGI_FORMAT_R8G8B8A8_TYPELESS:
      case DXGI_FORMAT_R8G8B8A8_UNORM:
      case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
      case DXGI_FORMAT_R8G8B8A8_UINT:
      case DXGI_FORMAT_R8G8B8A8_SNORM:
      case DXGI_FORMAT_R8G8B8A8_SINT:
      case DXGI_FORMAT_B8G8R8A8_UNORM:
      case DXGI_FORMAT_B8G8R8X8_UNORM:
      case DXGI_FORMAT_B8G8R8A8_TYPELESS:
      case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        return 4;

      case DXGI_FORMAT_R24G8_TYPELESS:
      case DXGI_FORMAT_D24_UNORM_S8_UINT:
        return 4;

      case DXGI_FORMAT_R8G8_TYPELESS:
      case DXGI_FORMAT_R8G8_UNORM:
      case DXGI_FORMAT_R8G8_UINT:
      case DXGI_FORMAT_R8G8_SNORM:
      case DXGI_FORMAT_R8G8_SINT:
        return 2;

      default:
        REX_ASSERT("Unknown DXGI type: {}", rsl::enum_refl::enum_name(format));
        return 0;
      }
    }

    s32 get_buffer_byte_size(const D3D12_RESOURCE_DESC& desc)
    {
      return desc.Width * desc.Height * format_byte_size(desc.Format);
    }
  }
} // namespace rex