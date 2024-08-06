#pragma once

// NOLINTBEGIN(llvm-include-order)
// clang-format off

#include "rex_engine/platform/win/win_com_ptr.h"

#include <d3d12.h>
#include <d3d11on12.h>
#include <d3d12shader.h>
#include "rex_directx/utility/d3dx12.h"

// clang-format on
// NOLINTEND(llvm-include-order)

#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/engine/types.h"
#include "rex_engine/memory/blob.h"
#include "rex_renderer_core/gfx/cull_mode.h"
#include "rex_renderer_core/gfx/fill_mode.h"
#include "rex_renderer_core/gfx/index_buffer_format.h"
#include "rex_renderer_core/gfx/input_layout_classification.h"
#include "rex_renderer_core/gfx/primitive_topology.h"
#include "rex_renderer_core/gfx/texture_format.h"
#include "rex_renderer_core/gfx/vertex_buffer_format.h"
#include "rex_renderer_core/gfx/shader_visibility.h"
#include "rex_renderer_core/gfx/sampler_filtering.h"
#include "rex_renderer_core/gfx/comparison_func.h"
#include "rex_renderer_core/gfx/border_color.h"
#include "rex_renderer_core/gfx/texture_address_mode.h"
#include "rex_renderer_core/resources/blend_state.h"
#include "rex_renderer_core/resources/depth_stencil_state.h"
#include "rex_renderer_core/gfx/texture_address_mode.h"
#include "rex_renderer_core/gfx/resource_state.h"
#include "rex_renderer_core/gfx/graphics_engine_type.h"
#include "rex_renderer_core/gfx/resource_state.h"
#include "rex_renderer_core/gfx/view_heap_type.h"
#include "rex_renderer_core/system/shader_elements.h"
#include "rex_renderer_core/shader_reflection/shader_class_type.h"
#include "rex_renderer_core/shader_reflection/shader_variable_type.h"
#include "rex_renderer_core/shader_reflection/shader_param_type.h"
#include "rex_renderer_core/shader_reflection/shader_param_reflection.h"
#include "rex_renderer_core/resources/raster_state.h"
#include "rex_std/bonus/utility.h"

namespace rex
{
  namespace gfx
  {
    struct ContextResetData;

    // Base classes
    class CommandQueue;
    class CommandAllocator;
    class RootSignature;
    class Shader;
    class Texture2D;
    class Sampler2D;
    class InputLayout;
    struct RasterStateDesc;
    enum class ShaderType;
    enum class ShaderVisibility;
    enum class ShaderResourceType;
    class IsColorNormalized;
    struct ViewTable;

    class ConstantBuffer;
    class IndexBuffer;
    class VertexBuffer;
    class UploadBuffer;
    class ViewHeap;
    class ShaderSignature;
    class ResourceView;
    class PipelineState;
    class Fence;
    class ConstantBuffer;
    class RenderTarget;
    class ShaderPipelineParameters;

    // DirectX classes
    class DxCommandQueue;
    class DxCommandAllocator;
    class DxRootSignature;
    class DxShader;
    class DxTexture2D;
    class DxInputLayout;
    class DxViewHeap;
    class DxSampler2D;
    class DxResourceView;
    class DxFence;
    class DxConstantBuffer;
    class DxVertexBuffer;
    class DxIndexBuffer;
    class DxUploadBuffer;
    class DxRenderTarget;
    class DxPipelineState;
    struct DxShaderPipelineParameters2;

    namespace d3d
    {
      // Useful function that assigns a name to a resource.
      // This is useful for debugging
      template<typename TResourceType>
      void set_debug_name_for(TResourceType* resource, rsl::string_view name)
      {
#ifdef REX_ENABLE_DEBUG_RESOURCE_NAMES
        resource->SetPrivateData(WKPDID_D3DDebugObjectName, name.length(), name.data());
#else
        REX_UNUSED_PARAM(resource);
        REX_UNUSED_PARAM(name);
#endif
    }

      // Return the ref count of a DirectX object
      template <typename TResourceType>
      s32 resource_ref_count(TResourceType* resource)
      {
        if (resource)
        {
          resource->AddRef();
          return resource->Release();
        }

        return 0;
      }

      // Return the ref count of a DirectX object
      template <typename TResourceType>
      s32 resource_ref_count(const wrl::ComPtr<TResourceType>& resource)
      {
        if (resource)
        {
          resource->AddRef();
          return resource->Release();
        }

        return 0;
      }
      
      // This isn't great as there isn't a way to pass the memory accross
      // from our blob to a d3d blob, we need to copy it over.
      // This means that memory will be duplicated.
      wrl::ComPtr<ID3DBlob> create_blob(const memory::Blob& blob);

      // Get the pitch size of a texture for a given format
      s32 texture_pitch_size(s32 width, TextureFormat format);
      // Get the total texture size in memory
      s32 total_texture_size(s32 width, s32 height, TextureFormat format);
      // Get the byte size of a given format
      s32 format_byte_size(DXGI_FORMAT format);

      // Based on the shader visibility flag, get the shader type
      ShaderType shader_visibility_to_type(ShaderVisibility visibility);

      // Reset a Direct X commandlist
      void reset_cmdlist(ID3D12GraphicsCommandList* cmdList, DxCommandAllocator* alloc, const ContextResetData& resetData);

      // ------------------------------------
      // Convertors from REX -> DirectX
      // ------------------------------------
      D3D12_FILL_MODE to_dx12(FillMode mode);
      D3D12_CULL_MODE to_dx12(CullMode mode);
      DXGI_FORMAT to_dx12(VertexBufferFormat format);
      DXGI_FORMAT to_dx12(IndexBufferFormat format);
      DXGI_FORMAT to_dx12(TextureFormat format);
      D3D12_PRIMITIVE_TOPOLOGY to_dx12(PrimitiveTopology topology);
      D3D12_INPUT_CLASSIFICATION to_dx12(InputLayoutClassification classification);
      D3D12_SHADER_VISIBILITY to_dx12(ShaderVisibility visibility);
      D3D12_FILTER to_dx12(SamplerFiltering filter);
      D3D12_COMPARISON_FUNC to_dx12(ComparisonFunc comparisonFunc);
      D3D12_STATIC_BORDER_COLOR to_dx12(BorderColor borderColor);
      D3D12_TEXTURE_ADDRESS_MODE to_dx12(TextureAddressMode addressMode);
      D3D12_RASTERIZER_DESC to_dx12(const RasterStateDesc& rasterState);
      D3D12_BLEND_DESC to_dx12(const BlendDesc& blendState);
      D3D12_BLEND to_dx12(Blend blend);
      D3D12_BLEND_OP to_dx12(BlendOp blendOp);
      D3D12_LOGIC_OP to_dx12(LogicOp logicOp);
      D3D12_DEPTH_STENCIL_DESC to_dx12(const DepthStencilDesc& depthStencilState);
      D3D12_DEPTH_WRITE_MASK to_dx12(DepthWriteMask mask);
      D3D12_DEPTH_STENCILOP_DESC to_dx12(const DepthStencilOpDesc& depthStencilOp);
      D3D12_STENCIL_OP to_dx12(StencilOp stencilOp);
      D3D12_DESCRIPTOR_RANGE to_dx12(ViewRangeDesc range);
      D3D12_DESCRIPTOR_RANGE_TYPE to_dx12(DescriptorRangeType type);
      D3D12_RESOURCE_STATES to_dx12(ResourceState state);
      D3D12_COMMAND_LIST_TYPE to_dx12(GraphicsEngineType type);
      D3D12_DESCRIPTOR_HEAP_TYPE to_dx12(ViewHeapType type);
      D3D12_PRIMITIVE_TOPOLOGY_TYPE to_dx12(PrimitiveTopologyType type);
      D3D12_SAMPLER_DESC to_dx12(const ShaderSamplerDesc& desc);
      D3D12_DESCRIPTOR_RANGE_TYPE to_dx12(ShaderResourceType type);

      // ------------------------------------
      // Converts from generic REX classes -> DirectX REX classes
      // ------------------------------------
      DxCommandQueue* to_dx12(CommandQueue* cmdQueue);
      DxCommandAllocator* to_dx12(CommandAllocator* cmdAlloc);
      DxRootSignature* to_dx12(RootSignature* rootSig);
      DxShader* to_dx12(Shader* shader);
      DxTexture2D* to_dx12(Texture2D* texture);
      DxInputLayout* to_dx12(InputLayout* inputLayout);
      DxViewHeap* to_dx12(ViewHeap* descHeap);
      DxSampler2D* to_dx12(Sampler2D* sampler);
      DxResourceView* to_dx12(ResourceView* resourceView);
      DxFence* to_dx12(Fence* fence);
      DxConstantBuffer* to_dx12(ConstantBuffer* constantBuffer);
      DxVertexBuffer* to_dx12(VertexBuffer* vertexBuffer);
      DxIndexBuffer* to_dx12(IndexBuffer* indexBuffer);
      DxUploadBuffer* to_dx12(UploadBuffer* uploadBuffer);
      DxRenderTarget* to_dx12(RenderTarget* renderTarget);
      DxPipelineState* to_dx12(PipelineState* pso);
      //DxShaderPipelineParameters2 to_dx12(const ShaderPipelineParameters& parameters);
      DxShaderPipelineParameters2 to_dx12(const rsl::vector<ViewTable>& parameters);

      const DxShader* to_dx12(const Shader* shader);

      // ------------------------------------
      // Returned the wrapped dx12 resource of a class
      // ------------------------------------
      ID3D12Resource* dx12_resource(ConstantBuffer* buffer);
      ID3D12Resource* dx12_resource(VertexBuffer* buffer);
      ID3D12Resource* dx12_resource(IndexBuffer* buffer);
      ID3D12Resource* dx12_resource(UploadBuffer* buffer);
      ID3D12Resource* dx12_resource(Texture2D* texture);
      ID3D12PipelineState* dx12_pso(PipelineState* pso);

      // ------------------------------------
      // Return from Directx -> REX
      // ------------------------------------
      ShaderParameterType from_dx12_shader_param_type(DXGI_FORMAT format);
      TextureFormat from_dx12(DXGI_FORMAT type);
      ShaderClassType from_dx12(D3D_SHADER_VARIABLE_CLASS type);
      ShaderVariableType from_dx12(D3D_SHADER_VARIABLE_TYPE type);
      ShaderVisibility from_dx12(D3D12_SHADER_VISIBILITY visibility);

    } // namespace d3d
  }
} // namespace rex