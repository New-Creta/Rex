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
#   define REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_INFO 0
#   define REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_WARNING 0
#   define REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_ERROR 1
#   define REX_ENABLE_DX12_BREAK_SEVERITY_LEVEL_CORRUPTION 1
#   define REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_MESSAGE 0
#   define REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_INFO 0
#   define REX_ENABLE_DXGI_BREAK_SEVERITY_LEVEL_WARNING 0
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
#include "rex_engine/engine/types.h"
#include "rex_engine/memory/blob.h"
#include "rex_renderer_core/rendering/cull_mode.h"
#include "rex_renderer_core/rendering/fill_mode.h"
#include "rex_renderer_core/rendering/index_buffer_format.h"
#include "rex_renderer_core/rendering/input_layout_classification.h"
#include "rex_renderer_core/rendering/primitive_topology.h"
#include "rex_renderer_core/rendering/texture_format.h"
#include "rex_renderer_core/rendering/vertex_buffer_format.h"
#include "rex_renderer_core/rendering/shader_visibility.h"
#include "rex_renderer_core/rendering/sampler_filtering.h"
#include "rex_renderer_core/rendering/comparison_func.h"
#include "rex_renderer_core/rendering/border_color.h"
#include "rex_renderer_core/rendering/texture_address_mode.h"
#include "rex_renderer_core/resources/blend_state.h"
#include "rex_renderer_core/resources/depth_stencil_state.h"
#include "rex_renderer_core/rendering/texture_address_mode.h"
#include "rex_renderer_core/resources/link_shader.h"
#include "rex_renderer_core/rendering/resource_state.h"
#include "rex_std/bonus/utility.h"

namespace rex
{
  namespace d3d
  {
    // This isn't great as there isn't a way to pass the memory accross
    // from our blob to a d3d blob, we need to copy it over.
    // This means that memory will be duplicated.
    wrl::ComPtr<ID3DBlob> create_blob(const memory::Blob& blob);
    D3D12_FILL_MODE to_dx12(renderer::FillMode mode);
    D3D12_CULL_MODE to_dx12(renderer::CullMode mode);
    DXGI_FORMAT to_dx12(renderer::VertexBufferFormat format);
    DXGI_FORMAT to_dx12(renderer::IndexBufferFormat format);
    DXGI_FORMAT to_dx12(renderer::TextureFormat format);
    D3D12_PRIMITIVE_TOPOLOGY to_dx12(renderer::PrimitiveTopology topology);
    D3D12_INPUT_CLASSIFICATION to_dx12(renderer::InputLayoutClassification classification);
    s32 format_byte_size(DXGI_FORMAT format);
    s32 texture_2d_size(DXGI_FORMAT format, s32 width, s32 height);
    D3D12_SHADER_VISIBILITY to_dx12(renderer::ShaderVisibility visibility);
    D3D12_FILTER to_dx12(renderer::SamplerFiltering filter);
    D3D12_COMPARISON_FUNC to_dx12(renderer::ComparisonFunc comparisonFunc);
    D3D12_STATIC_BORDER_COLOR to_dx12(renderer::BorderColor borderColor);
    D3D12_TEXTURE_ADDRESS_MODE to_dx12(renderer::TextureAddressMode addressMode);

    D3D12_BLEND_DESC to_dx12(const rhi::BlendDesc& blendState);
    D3D12_BLEND to_dx12(rhi::Blend blend);
    D3D12_BLEND_OP to_dx12(rhi::BlendOp blendOp);
    D3D12_LOGIC_OP to_dx12(rhi::LogicOp logicOp);
    D3D12_DEPTH_STENCIL_DESC to_dx12(const rhi::DepthStencilDesc& depthStencilState);
    D3D12_DEPTH_WRITE_MASK to_dx12(rhi::DepthWriteMask mask);
    D3D12_DEPTH_STENCILOP_DESC to_dx12(const rhi::DepthStencilOpDesc& depthStencilOp);
    D3D12_STENCIL_OP to_dx12(rhi::StencilOp stencilOp);
    D3D12_DESCRIPTOR_RANGE to_dx12(rhi::DescriptorRangeDesc range);
    D3D12_DESCRIPTOR_RANGE_TYPE to_dx12(rhi::DescriptorRangeType type);
    D3D12_RESOURCE_STATES to_dx12(ResourceState state);
  } // namespace d3d
} // namespace rex