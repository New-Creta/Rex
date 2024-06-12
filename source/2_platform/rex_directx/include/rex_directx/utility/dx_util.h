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

namespace rex 
{ 
  namespace globals 
  { 
#ifdef REX_ENABLE_GFX_DEBUGGING
    constexpr bool g_enable_dx12_severity_message     = false;
    constexpr bool g_enable_dx12_severity_info        = false;
    constexpr bool g_enable_dx12_severity_warning     = false;
    constexpr bool g_enable_dx12_severity_error       = true;
    constexpr bool g_enable_dx12_severity_corruption  = true;
    constexpr bool g_enable_dxgi_severity_message     = false;
    constexpr bool g_enable_dxgi_severity_info        = false;
    constexpr bool g_enable_dxgi_severity_warning     = false;
    constexpr bool g_enable_dxgi_severity_error       = true;
    constexpr bool g_enable_dxgi_severity_corruption  = true;
#else
    constexpr bool g_enable_dx12_severity_message     = false;
    constexpr bool g_enable_dx12_severity_info        = false;
    constexpr bool g_enable_dx12_severity_warning     = false;
    constexpr bool g_enable_dx12_severity_error       = false;
    constexpr bool g_enable_dx12_severity_corruption  = false;
    constexpr bool g_enable_dxgi_severity_message     = false;
    constexpr bool g_enable_dxgi_severity_info        = false;
    constexpr bool g_enable_dxgi_severity_warning     = false;
    constexpr bool g_enable_dxgi_severity_error       = false;
    constexpr bool g_enable_dxgi_severity_corruption  = false;
#endif
  } // namespace globals 
} // namespace rex

// clang-format on
// NOLINTEND(llvm-include-order)

#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/engine/types.h"
#include "rex_engine/memory/blob.h"
#include "rex_renderer_core/rhi/cull_mode.h"
#include "rex_renderer_core/rhi/fill_mode.h"
#include "rex_renderer_core/rhi/index_buffer_format.h"
#include "rex_renderer_core/rhi/input_layout_classification.h"
#include "rex_renderer_core/rhi/primitive_topology.h"
#include "rex_renderer_core/rhi/texture_format.h"
#include "rex_renderer_core/rhi/vertex_buffer_format.h"
#include "rex_renderer_core/rhi/shader_visibility.h"
#include "rex_renderer_core/rhi/sampler_filtering.h"
#include "rex_renderer_core/rhi/comparison_func.h"
#include "rex_renderer_core/rhi/border_color.h"
#include "rex_renderer_core/rhi/texture_address_mode.h"
#include "rex_renderer_core/resources/blend_state.h"
#include "rex_renderer_core/resources/depth_stencil_state.h"
#include "rex_renderer_core/rhi/texture_address_mode.h"
#include "rex_renderer_core/rhi/resource_state.h"
#include "rex_renderer_core/rhi/graphics_engine_type.h"
#include "rex_renderer_core/rhi/resource_state.h"
#include "rex_renderer_core/system/link_shader.h"
#include "rex_std/bonus/utility.h"

namespace rex
{
  namespace rhi
  {
    // Base classes
    class CommandQueue;
    class CommandAllocator;
    class RootSignature;
    class Shader;
    class Texture2D;
    class InputLayout;
    struct RasterStateDesc;

    class ConstantBuffer;
    class IndexBuffer;
    class VertexBuffer;
    class UploadBuffer;

    // DirectX classes
    class DxCommandQueue;
    class DxCommandAllocator;
    class DxRootSignature;
    class DxShader;
    class DxTexture2D;
    class DxInputLayoutResource;
  }



  namespace d3d
  {
    // This isn't great as there isn't a way to pass the memory accross
    // from our blob to a d3d blob, we need to copy it over.
    // This means that memory will be duplicated.
    wrl::ComPtr<ID3DBlob> create_blob(const memory::Blob& blob);
    s32 texture_pitch_size(s32 width, renderer::TextureFormat format);
    s32 total_texture_size(s32 width, s32 height, renderer::TextureFormat format);

    D3D12_FILL_MODE to_dx12(renderer::FillMode mode);
    D3D12_CULL_MODE to_dx12(renderer::CullMode mode);
    DXGI_FORMAT to_dx12(renderer::VertexBufferFormat format);
    DXGI_FORMAT to_dx12(renderer::IndexBufferFormat format);
    DXGI_FORMAT to_dx12(renderer::TextureFormat format);
    D3D12_PRIMITIVE_TOPOLOGY to_dx12(renderer::PrimitiveTopology topology);
    D3D12_INPUT_CLASSIFICATION to_dx12(renderer::InputLayoutClassification classification);
    s32 format_byte_size(renderer::TextureFormat format);
    s32 format_byte_size(DXGI_FORMAT format);
    s32 texture_2d_size(DXGI_FORMAT format, s32 width, s32 height);
    D3D12_SHADER_VISIBILITY to_dx12(renderer::ShaderVisibility visibility);
    D3D12_FILTER to_dx12(renderer::SamplerFiltering filter);
    D3D12_COMPARISON_FUNC to_dx12(renderer::ComparisonFunc comparisonFunc);
    D3D12_STATIC_BORDER_COLOR to_dx12(renderer::BorderColor borderColor);
    D3D12_TEXTURE_ADDRESS_MODE to_dx12(renderer::TextureAddressMode addressMode);

    D3D12_RASTERIZER_DESC to_dx12(const rhi::RasterStateDesc& rasterState);
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
    D3D12_RESOURCE_STATES to_dx12(rhi::ResourceState state);
    D3D12_COMMAND_LIST_TYPE to_dx12(rhi::GraphicsEngineType type);

    rhi::DxCommandQueue* to_dx12(rhi::CommandQueue* cmdQueue);
    rhi::DxCommandAllocator* to_dx12(rhi::CommandAllocator* cmdAlloc);
    rhi::DxRootSignature* to_dx12(rhi::RootSignature* rootSig);
    rhi::DxShader* to_dx12(rhi::Shader* shader);
    rhi::DxTexture2D* to_dx12(rhi::Texture2D* texture);
    rhi::DxInputLayoutResource* to_dx12(rhi::InputLayout* inputLayout);

    ID3D12Resource* dx12_resource(rhi::ConstantBuffer* buffer);
    ID3D12Resource* dx12_resource(rhi::VertexBuffer* buffer);
    ID3D12Resource* dx12_resource(rhi::IndexBuffer* buffer);
    ID3D12Resource* dx12_resource(rhi::UploadBuffer* buffer);
    ID3D12Resource* dx12_resource(rhi::Texture2D* texture);


    rhi::GraphicsEngineType from_dx12(D3D12_COMMAND_LIST_TYPE type);
    renderer::TextureFormat from_dx12(DXGI_FORMAT type);
  } // namespace d3d
} // namespace rex