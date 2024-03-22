#pragma once

// NOLINTBEGIN(llvm-include-order)
// clang-format off

#include "rex_engine/platform/win/win_com_ptr.h"

#include <d3d12.h>
#include <d3d11on12.h>

// Link necessary d3d12 libraries.
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")


  
    

    
  


#if defined(REX_BUILD_DEBUG) || defined(REX_BUILD_DEBUG_OPT)
#define REX_ENABLE_GFX_DEBUGGING
#endif

#ifdef REX_ENABLE_GFX_DEBUGGING
#   define REX_ENABLE_DEBUG_SHADER_COMPILATION 1
#   define REX_ENABLE_DEBUG_RESOURCE_NAMES 1
#   define REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_INFO 1
#   define REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_WARNING 1
#   define REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_ERROR 1
#   define REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_CORRUPTION 1
#   define REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_MESSAGE 1
#   define REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_INFO 1
#   define REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_WARNING 1
#   define REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_ERROR 1
#   define REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_CORRUPTION 1
#   define REX_ENABLE_DXGI_DEBUG_LAYER 1
#   define REX_ENABLE_DXGI_LIVE_OBJECT_REPORT 1
#   define REX_ENABLE_DX12_DEBUG_LAYER 1
#   define REX_ENABLE_DX12_LIVE_OBJECT_REPORT 1
#   define REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_MESSAGE 1
# else
#   define REX_ENABLE_DEBUG_SHADER_COMPILATION 0
#   define REX_ENABLE_DEBUG_RESOURCE_NAMES 0
#   define REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_INFO 0
#   define REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_WARNING 0
#   define REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_ERROR 0
#   define REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_CORRUPTION 0
#   define REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_MESSAGE 0
#   define REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_INFO 0
#   define REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_WARNING 0
#   define REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_ERROR 0
#   define REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_CORRUPTION 0
#   define REX_ENABLE_DXGI_DEBUG_LAYER 0
#   define REX_ENABLE_DXGI_LIVE_OBJECT_REPORT 0
#   define REX_ENABLE_DX12_DEBUG_LAYER 0
#   define REX_ENABLE_DX12_LIVE_OBJECT_REPORT 0
#   define REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_MESSAGE 0
#endif

namespace rex 
{ 
  namespace globals 
  { 
    constexpr bool g_enable_dx12_severity_message     = REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_MESSAGE; 
    constexpr bool g_enable_dx12_severity_info        = REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_INFO;
    constexpr bool g_enable_dx12_severity_warning     = REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_WARNING;
    constexpr bool g_enable_dx12_severity_error       = REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_ERROR;
    constexpr bool g_enable_dx12_severity_corruption  = REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_CORRUPTION;
    constexpr bool g_enable_dxgi_severity_message     = REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_MESSAGE;
    constexpr bool g_enable_dxgi_severity_info        = REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_INFO;
    constexpr bool g_enable_dxgi_severity_warning     = REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_WARNING;
    constexpr bool g_enable_dxgi_severity_error       = REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_ERROR;
    constexpr bool g_enable_dxgi_severity_corruption  = REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_CORRUPTION;
  } // namespace globals 
} // namespace rex

// clang-format on
// NOLINTEND(llvm-include-order)

#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/memory/blob.h"
#include "rex_renderer_core/rendering/fill_mode.h"
#include "rex_renderer_core/rendering/cull_mode.h"
#include "rex_renderer_core/rendering/vertex_buffer_format.h"
#include "rex_renderer_core/rendering/index_buffer_format.h"
#include "rex_renderer_core/rendering/texture_format.h"
#include "rex_renderer_core/rendering/primitive_topology.h"
#include "rex_renderer_core/rendering/input_layout_classification.h"

namespace rex
{
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
      case renderer::IndexBufferFormat::R16Uint: return DXGI_FORMAT_R16_UINT;
      case renderer::IndexBufferFormat::R32Uint: return DXGI_FORMAT_R32_UINT;
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
  }
}