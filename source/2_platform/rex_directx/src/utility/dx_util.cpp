#include "rex_directx/utility/dx_util.h"
#include "rex_directx/utility/d3dx12.h"

#include "rex_engine/gfx/core/index_buffer_format.h"
#include "rex_engine/memory/pointer_math.h"
#include "rex_engine/engine/invalid_object.h"

#include "rex_std/bonus/utility.h"

#include <d3dcompiler.h>

#include "rex_directx/system/dx_command_queue.h"
#include "rex_directx/system/dx_command_allocator.h"
#include "rex_directx/system/dx_shader.h"
#include "rex_directx/resources/dx_constant_buffer.h"
#include "rex_directx/resources/dx_root_signature.h"
#include "rex_directx/resources/dx_texture_2d.h"
#include "rex_directx/resources/dx_sampler_2d.h"
#include "rex_directx/resources/dx_input_layout.h"

#include "rex_directx/resources/dx_constant_buffer.h"
#include "rex_directx/resources/dx_vertex_buffer.h"
#include "rex_directx/resources/dx_index_buffer.h"
#include "rex_directx/resources/dx_upload_buffer.h"
#include "rex_directx/resources/dx_pipeline_state.h"
#include "rex_directx/resources/dx_render_target.h"
#include "rex_directx/resources/dx_sampler_2d.h"
#include "rex_directx/resources/dx_depth_stencil_buffer.h"
#include "rex_directx/system/dx_view_heap.h"
#include "rex_engine/gfx/system/shader_param_declaration.h"

#include "rex_engine/gfx/shader_reflection/shader_signature.h"

#include "rex_engine/gfx/materials/material_system.h"
#include "rex_engine/gfx/system/rhi.h"
#include "rex_engine/gfx/system/graphics_engine.h"

#include "rex_directx/system/dx_shader_root_parameters.h"

namespace rex
{
  namespace gfx
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

      s32 texture_pitch_size(s32 width, TextureFormat format)
      {
        s32 format_size = format_byte_size(format);
        s32 pitch_size = width * format_size;
        
        return pitch_size;
      }

      s32 aligned_texture_pitch_size(s32 width, TextureFormat format)
      {
        s32 format_size = format_byte_size(format);
        s32 pitch_size = width * format_size;
        s32 alignment = D3D12_TEXTURE_DATA_PITCH_ALIGNMENT;
        pitch_size = align(pitch_size, alignment);

        return pitch_size;
      }
      s32 total_texture_size(s32 width, s32 height, TextureFormat format)
      {
        const s32 format_size = format_byte_size(format);
        const s32 alignment = D3D12_TEXTURE_DATA_PITCH_ALIGNMENT;
        s32 pitch_size = width * format_size;
        pitch_size = align(pitch_size, alignment);

        return pitch_size * height;
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

        case DXGI_FORMAT_R32G32_FLOAT:
          return 8;

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

        case DXGI_FORMAT_R8_UINT:
        case DXGI_FORMAT_R8_UNORM:
        case DXGI_FORMAT_R8_SINT:
        case DXGI_FORMAT_R8_SNORM:
          return 1;

        default:
          REX_ASSERT("Unknown DXGI type: {}", rsl::enum_refl::enum_name(format));
          return 0;
        }
      }

      ShaderType shader_visibility_to_type(ShaderVisibility visibility)
      {
        switch (visibility)
        {
        case rex::gfx::ShaderVisibility::Vertex:          return ShaderType::Vertex;
        case rex::gfx::ShaderVisibility::Pixel:           return ShaderType::Pixel;
        case rex::gfx::ShaderVisibility::Geometry:        return ShaderType::Geometry;
        case rex::gfx::ShaderVisibility::Hull:            return ShaderType::Hull;
        case rex::gfx::ShaderVisibility::Domain:          return ShaderType::Domain;
        case rex::gfx::ShaderVisibility::Amplification:   return ShaderType::Amplification;
        case rex::gfx::ShaderVisibility::Mesh:            return ShaderType::Mesh;
        default: break;
        }

        return invalid_obj<ShaderType>();
      }

      void reset_cmdlist(ID3D12GraphicsCommandList* cmdList, DxCommandAllocator* alloc, const ContextResetData& resetData)
      {
        REX_ASSERT_X(alloc != nullptr, "The command allocator for a context cannot be null");

        alloc->dx_object()->Reset();
        cmdList->Reset(alloc->dx_object(), d3d::dx12_pso(resetData.pso));

        rsl::array<ID3D12DescriptorHeap*, 2> d3d_desc_heaps{};
        d3d_desc_heaps[0] = d3d::to_dx12(resetData.shader_visible_srv_desc_heap)->dx_object();
        d3d_desc_heaps[1] = d3d::to_dx12(resetData.shader_visible_sampler_desc_heap)->dx_object();
        cmdList->SetDescriptorHeaps(d3d_desc_heaps.size(), d3d_desc_heaps.data());
      }


      //-------------------------------------------------------------------------
      // CONVERTORS
      //-------------------------------------------------------------------------

      // ------------------------------------
      // Convertors from REX -> DirectX
      // ------------------------------------
      D3D12_FILL_MODE to_dx12(FillMode mode)
      {
        switch (mode)
        {
        case FillMode::Solid: return D3D12_FILL_MODE_SOLID;
        case FillMode::Wireframe: return D3D12_FILL_MODE_WIREFRAME;
        default: break;
        }

        REX_ASSERT("Unsupported fill mode given");
        return D3D12_FILL_MODE_SOLID;
      }
      D3D12_CULL_MODE to_dx12(CullMode mode)
      {
        switch (mode)
        {
        case CullMode::None: return D3D12_CULL_MODE_NONE;
        case CullMode::Front: return D3D12_CULL_MODE_FRONT;
        case CullMode::Back: return D3D12_CULL_MODE_BACK;
        default: break;
        }

        REX_ASSERT("Unsupported cull mode given");
        return D3D12_CULL_MODE_NONE;
      }
      DXGI_FORMAT to_dx12(VertexBufferFormat format)
      {
        switch (format)
        {
          // 4 component types
        case ShaderArithmeticType::Float4:        return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case ShaderArithmeticType::Uint4:         return DXGI_FORMAT_R32G32B32A32_UINT;
        case ShaderArithmeticType::Int4:          return DXGI_FORMAT_R32G32B32A32_SINT;
        
        case ShaderArithmeticType::Half4:         return DXGI_FORMAT_R16G16B16A16_FLOAT;
        case ShaderArithmeticType::Ushort4:       return DXGI_FORMAT_R16G16B16A16_UINT;
        case ShaderArithmeticType::Short4:        return DXGI_FORMAT_R16G16B16A16_SINT;
        
        case ShaderArithmeticType::UChar4:        return DXGI_FORMAT_R8G8B8A8_UINT;
        case ShaderArithmeticType::UChar4Norm:    return DXGI_FORMAT_R8G8B8A8_UNORM;
        case ShaderArithmeticType::Char4:         return DXGI_FORMAT_R8G8B8A8_SINT;
        case ShaderArithmeticType::Char4Norm:     return DXGI_FORMAT_R8G8B8A8_SNORM;

          // 3 component types
        case ShaderArithmeticType::Float3:        return DXGI_FORMAT_R32G32B32_FLOAT; 
        case ShaderArithmeticType::Uint3:         return DXGI_FORMAT_R32G32B32_UINT;
        case ShaderArithmeticType::Int3:          return DXGI_FORMAT_R32G32B32_SINT;

          // 2 component types
        case ShaderArithmeticType::Float2:        return DXGI_FORMAT_R32G32_FLOAT;
        case ShaderArithmeticType::Uint2:         return DXGI_FORMAT_R32G32_UINT;
        case ShaderArithmeticType::Int2:          return DXGI_FORMAT_R32G32_SINT;

        case ShaderArithmeticType::Half2:         return DXGI_FORMAT_R16G16_FLOAT;
        case ShaderArithmeticType::Ushort2:       return DXGI_FORMAT_R16G16_UINT;
        case ShaderArithmeticType::Ushort2Norm:   return DXGI_FORMAT_R16G16_UNORM;
        case ShaderArithmeticType::Short2:        return DXGI_FORMAT_R16G16_SINT;
        case ShaderArithmeticType::Short2Norm:    return DXGI_FORMAT_R16G16_SNORM;

        case ShaderArithmeticType::UChar2:        return DXGI_FORMAT_R8G8_UINT;
        case ShaderArithmeticType::UChar2Norm:    return DXGI_FORMAT_R8G8_UNORM;
        case ShaderArithmeticType::Char2:         return DXGI_FORMAT_R8G8_SINT;
        case ShaderArithmeticType::Char2Norm:     return DXGI_FORMAT_R8G8_SNORM;

          // 1 component types
        case ShaderArithmeticType::Float:         return DXGI_FORMAT_R32_FLOAT;
        case ShaderArithmeticType::Uint:          return DXGI_FORMAT_R32_UINT;
        case ShaderArithmeticType::Int:           return DXGI_FORMAT_R32_SINT;

        case ShaderArithmeticType::Half:          return DXGI_FORMAT_R16_FLOAT;
        case ShaderArithmeticType::Ushort:        return DXGI_FORMAT_R16_UINT;
        case ShaderArithmeticType::UshortNorm:    return DXGI_FORMAT_R16_UNORM;
        case ShaderArithmeticType::Short:         return DXGI_FORMAT_R16_SINT;
        case ShaderArithmeticType::ShortNorm:     return DXGI_FORMAT_R16_SNORM;

        case ShaderArithmeticType::UChar:         return DXGI_FORMAT_R8_SINT;
        case ShaderArithmeticType::UCharNorm:     return DXGI_FORMAT_R8_SNORM;
        case ShaderArithmeticType::Char:          return DXGI_FORMAT_R8_UINT;
        case ShaderArithmeticType::CharNorm:      return DXGI_FORMAT_R8_UNORM;

        case ShaderArithmeticType::Bool:          return DXGI_FORMAT_R1_UNORM;

        default: break;
        }

        REX_ASSERT("Unsupported vertex buffer format given");
        return DXGI_FORMAT_UNKNOWN;
      }
      DXGI_FORMAT to_dx12(IndexBufferFormat format)
      {
        switch (format)
        {
        case IndexBufferFormat::Uint16: return DXGI_FORMAT_R16_UINT;
        case IndexBufferFormat::Uint32: return DXGI_FORMAT_R32_UINT;
        default: break;
        }
        REX_ASSERT("Unsupported index buffer format given");
        return DXGI_FORMAT_UNKNOWN;
      }
      DXGI_FORMAT to_dx12(TextureFormat format)
      {
        switch (format)
        {
        case TextureFormat::Unknown: return DXGI_FORMAT_UNKNOWN;
        case TextureFormat::Unorm4Srgb: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        case TextureFormat::Unorm4: return DXGI_FORMAT_R8G8B8A8_UNORM;
        case TextureFormat::Depth32: return DXGI_FORMAT_D32_FLOAT;
        case TextureFormat::Depth24Stencil8: return DXGI_FORMAT_D24_UNORM_S8_UINT;
        default: break;
        }
        REX_ASSERT("Unsupported vertex buffer format given");
        return DXGI_FORMAT_UNKNOWN;
      }
      D3D12_PRIMITIVE_TOPOLOGY to_dx12(PrimitiveTopology topology)
      {
        switch (topology)
        {
        case PrimitiveTopology::LineList: return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
        case PrimitiveTopology::LineStrip: return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
        case PrimitiveTopology::PointList: return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
        case PrimitiveTopology::TriangleList: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        case PrimitiveTopology::TriangleStrip: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
        default: break;
        }
        REX_ASSERT("Unsupported primitive topology given");
        return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
      }
      D3D12_INPUT_CLASSIFICATION to_dx12(InputLayoutClassification classification)
      {
        switch (classification)
        {
        case InputLayoutClassification::PerVertex: return D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        case InputLayoutClassification::PerInstance: return D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
        default: break;
        }

        REX_ASSERT("Unsupported input layout classification given");
        return D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
      }
      D3D12_SHADER_VISIBILITY to_dx12(ShaderVisibility visibility)
      {
        switch (visibility)
        {
        case ShaderVisibility::Vertex: return D3D12_SHADER_VISIBILITY_VERTEX;
        case ShaderVisibility::Pixel: return D3D12_SHADER_VISIBILITY_PIXEL;
        case ShaderVisibility::Amplification: return D3D12_SHADER_VISIBILITY_AMPLIFICATION;
        case ShaderVisibility::Domain: return D3D12_SHADER_VISIBILITY_DOMAIN;
        case ShaderVisibility::Geometry: return D3D12_SHADER_VISIBILITY_GEOMETRY;
        case ShaderVisibility::Hull: return D3D12_SHADER_VISIBILITY_HULL;
        case ShaderVisibility::Mesh: return D3D12_SHADER_VISIBILITY_MESH;
        case ShaderVisibility::All: return D3D12_SHADER_VISIBILITY_ALL;
        }

        REX_ASSERT("Unsupported shader visibility for directx 12: {}", rsl::enum_refl::enum_name(visibility));
        return invalid_obj<D3D12_SHADER_VISIBILITY>();
      }
      D3D12_FILTER to_dx12(SamplerFiltering filter)
      {
        switch (filter)
        {
        case SamplerFiltering::MinMagMipPoint:                                      return D3D12_FILTER_MIN_MAG_MIP_POINT;
        case SamplerFiltering::MinMagPointMipLinear:                                return D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
        case SamplerFiltering::MinPointMagLinearMipPoint:                           return D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
        case SamplerFiltering::MinPointMagMipLinear:                                return D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR;
        case SamplerFiltering::MinLinearMagMipPoint:                                return D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;
        case SamplerFiltering::MinLinearMagPointMipLinear:                          return D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
        case SamplerFiltering::MinMagLinearMipPoint:                                return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        case SamplerFiltering::MinMagMipLinear:                                     return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        case SamplerFiltering::Anisotropic:                                         return D3D12_FILTER_ANISOTROPIC;
        case SamplerFiltering::ComparisonMinMagMipPoint:                            return D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
        case SamplerFiltering::ComparisonMinMagPointMipLinear:                      return D3D12_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
        case SamplerFiltering::ComparisonMinPointMagLinearMipPoint:                 return D3D12_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
        case SamplerFiltering::ComparisonMinPointMagMipLinear:                      return D3D12_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
        case SamplerFiltering::ComparisonMinLinearMagMipPoint:                      return D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
        case SamplerFiltering::ComparisonMinLinearMagPointMipLinear:                return D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
        case SamplerFiltering::ComparisonMinMagLinearMipPoint:                      return D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
        case SamplerFiltering::ComparisonMinMagMipLinear:                           return D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
        case SamplerFiltering::ComparisonAnisotropic:                               return D3D12_FILTER_COMPARISON_ANISOTROPIC;
        case SamplerFiltering::MinimumMinMagMipPoint:                               return D3D12_FILTER_MINIMUM_MIN_MAG_MIP_POINT;
        case SamplerFiltering::MinimumMinMagPointMipLinear:                         return D3D12_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR;
        case SamplerFiltering::MinimumMinPointMagLinearMipPoint:                    return D3D12_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
        case SamplerFiltering::MinimumMinPointMagMipLinear:                         return D3D12_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR;
        case SamplerFiltering::MinimumMinLinearMagMipPoint:                         return D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT;
        case SamplerFiltering::MinimumMinLinearMagPointMipLinear:                   return D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
        case SamplerFiltering::MinimumMinMagLinearMipPoint:                         return D3D12_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT;
        case SamplerFiltering::MinimumMinMagMipLinear:                              return D3D12_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR;
        case SamplerFiltering::MinimumAnisotropic:                                  return D3D12_FILTER_MINIMUM_ANISOTROPIC;
        case SamplerFiltering::MaximumMinMagMipPoint:                               return D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_POINT;
        case SamplerFiltering::MaximumMinMagPointMipLinear:                         return D3D12_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR;
        case SamplerFiltering::MaximumMinPointMagLinearMipPoint:                    return D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
        case SamplerFiltering::MaximumMinPointMagMipLinear:                         return D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR;
        case SamplerFiltering::MaximumMinLinearMagMipPoint:                         return D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT;
        case SamplerFiltering::MaximumMinLinearMagPointMipLinear:                   return D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
        case SamplerFiltering::MaximumMinMagLinearMipPoint:                         return D3D12_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT;
        case SamplerFiltering::MaximumMinMagMipLinear:                              return D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR;
        case SamplerFiltering::MaximumAnisotropic:                                  return D3D12_FILTER_MAXIMUM_ANISOTROPIC;
        }

        REX_ASSERT("Unsupported sampler filter for directx 12: {}", rsl::enum_refl::enum_name(filter));
        return invalid_obj<D3D12_FILTER>();
      }
      D3D12_COMPARISON_FUNC to_dx12(ComparisonFunc comparisonFunc)
      {
        switch (comparisonFunc)
        {
        case ComparisonFunc::Never: return D3D12_COMPARISON_FUNC_NEVER;
        case ComparisonFunc::Always: return D3D12_COMPARISON_FUNC_ALWAYS;
        case ComparisonFunc::Less: return D3D12_COMPARISON_FUNC_LESS;
        case ComparisonFunc::LessEqual: return D3D12_COMPARISON_FUNC_LESS_EQUAL;
        case ComparisonFunc::Equal: return D3D12_COMPARISON_FUNC_EQUAL;
        case ComparisonFunc::Greater: return D3D12_COMPARISON_FUNC_GREATER;
        case ComparisonFunc::GreaterEqual: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
        case ComparisonFunc::NotEqual: return D3D12_COMPARISON_FUNC_NOT_EQUAL;
        }

        REX_ASSERT("Unsupported comparison func for directx 12: {}", rsl::enum_refl::enum_name(comparisonFunc));
        return invalid_obj<D3D12_COMPARISON_FUNC>();
      }
      D3D12_STATIC_BORDER_COLOR to_dx12(BorderColor borderColor)
      {
        switch (borderColor)
        {
        case BorderColor::TransparentBlack: return D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        case BorderColor::OpaqueBlack: return D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
        case BorderColor::OpaqueWhite: return D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
        }

        REX_ASSERT("Unsupported border color for directx 12: {}", rsl::enum_refl::enum_name(borderColor));
        return invalid_obj<D3D12_STATIC_BORDER_COLOR>();
      }
      D3D12_TEXTURE_ADDRESS_MODE to_dx12(TextureAddressMode addressMode)
      {
        switch (addressMode)
        {
        case TextureAddressMode::Wrap: return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        case TextureAddressMode::Mirror: return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
        case TextureAddressMode::Clamp: return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        case TextureAddressMode::Border: return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        case TextureAddressMode::MirrorOnce: return D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
        }

        REX_ASSERT("Unsupported addressmode for directx 12: {}", rsl::enum_refl::enum_name(addressMode));
        return invalid_obj<D3D12_TEXTURE_ADDRESS_MODE>();
      }
      D3D12_RASTERIZER_DESC to_dx12(const RasterStateDesc& rasterState)
      {
        D3D12_RASTERIZER_DESC desc{};
        desc.FillMode = to_dx12(rasterState.fill_mode);
        desc.CullMode = to_dx12(rasterState.cull_mode);
        desc.FrontCounterClockwise = rasterState.front_ccw;
        desc.DepthBias = rasterState.depth_bias;
        desc.DepthBiasClamp = rasterState.depth_bias_clamp;
        desc.SlopeScaledDepthBias = rasterState.sloped_scale_depth_bias;
        desc.DepthClipEnable = rasterState.depth_clip_enable;
        desc.MultisampleEnable = rasterState.multisample_enable;
        desc.AntialiasedLineEnable = rasterState.aa_lines_enable;
        desc.ForcedSampleCount = rasterState.forced_sample_count;
        desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

        return desc;
      }
      D3D12_BLEND_DESC to_dx12(const BlendDesc& blendState)
      {
        D3D12_BLEND_DESC desc{};
        desc.AlphaToCoverageEnable = blendState.enable_alpha_to_coverage;
        desc.IndependentBlendEnable = blendState.independent_blend_state;

        for (s32 i = 0; i < rsl::size(desc.RenderTarget); ++i)
        {
          desc.RenderTarget[i].BlendEnable = blendState.render_target[i].blend_enable;
          desc.RenderTarget[i].SrcBlend = to_dx12(blendState.render_target[i].src_blend);
          desc.RenderTarget[i].DestBlend = to_dx12(blendState.render_target[i].dst_blend);
          desc.RenderTarget[i].BlendOp = to_dx12(blendState.render_target[i].blend_op);
          desc.RenderTarget[i].SrcBlendAlpha = to_dx12(blendState.render_target[i].src_blend_alpha);
          desc.RenderTarget[i].DestBlendAlpha = to_dx12(blendState.render_target[i].dst_blend_alpha);
          desc.RenderTarget[i].BlendOpAlpha = to_dx12(blendState.render_target[i].blend_op_alpha);
          desc.RenderTarget[i].LogicOpEnable = blendState.render_target[i].logic_op_enable;
          desc.RenderTarget[i].LogicOp = to_dx12(blendState.render_target[i].logic_op);
          desc.RenderTarget[i].RenderTargetWriteMask = static_cast<uint8>(blendState.render_target[i].render_target_write_mask);
        }

        return desc;
      }
      D3D12_BLEND to_dx12(Blend blend)
      {
        switch (blend)
        {
        case Blend::Zero:           return D3D12_BLEND_ZERO;
        case Blend::One:            return D3D12_BLEND_ONE;
        case Blend::SrcColor:       return D3D12_BLEND_SRC_COLOR;
        case Blend::InvSrcColor:    return D3D12_BLEND_INV_SRC_COLOR;
        case Blend::SrcAlpha:       return D3D12_BLEND_SRC_ALPHA;
        case Blend::InvSrcAlpha:    return D3D12_BLEND_INV_SRC_ALPHA;
        case Blend::DestAlpha:      return D3D12_BLEND_DEST_ALPHA;
        case Blend::InvDestAlpha:   return D3D12_BLEND_INV_DEST_ALPHA;
        case Blend::DestColor:      return D3D12_BLEND_DEST_COLOR;
        case Blend::InvDestColor:   return D3D12_BLEND_INV_DEST_COLOR;
        case Blend::SrcAlphaSat:    return D3D12_BLEND_SRC_ALPHA_SAT;
        case Blend::BlendFactor:    return D3D12_BLEND_BLEND_FACTOR;
        case Blend::InvBlendFactor: return D3D12_BLEND_INV_BLEND_FACTOR;
        case Blend::Src1Color:      return D3D12_BLEND_SRC1_COLOR;
        case Blend::InvSrc1Color:   return D3D12_BLEND_INV_SRC1_COLOR;
        case Blend::Src1Alpha:      return D3D12_BLEND_SRC1_ALPHA;
        case Blend::InvSrc1Alpha:   return D3D12_BLEND_INV_SRC1_ALPHA;
        default:
          break;
        }

        return invalid_obj<D3D12_BLEND>();
      }
      D3D12_BLEND_OP to_dx12(BlendOp blendOp)
      {
        switch (blendOp)
        {
        case BlendOp::Add:          return D3D12_BLEND_OP_ADD;
        case BlendOp::Subtract:     return D3D12_BLEND_OP_SUBTRACT;
        case BlendOp::RevSubtract:  return D3D12_BLEND_OP_REV_SUBTRACT;
        case BlendOp::Min:          return D3D12_BLEND_OP_MIN;
        case BlendOp::Max:          return D3D12_BLEND_OP_MAX;
        }

        return invalid_obj<D3D12_BLEND_OP>();
      }
      D3D12_LOGIC_OP to_dx12(LogicOp logicOp)
      {
        switch (logicOp)
        {
        case LogicOp::Clear:          return D3D12_LOGIC_OP_CLEAR;
        case LogicOp::Set:            return D3D12_LOGIC_OP_SET;
        case LogicOp::Copy:           return D3D12_LOGIC_OP_COPY;
        case LogicOp::CopyInverted:   return D3D12_LOGIC_OP_COPY_INVERTED;
        case LogicOp::Noop:           return D3D12_LOGIC_OP_NOOP;
        case LogicOp::Invert:            return D3D12_LOGIC_OP_INVERT;
        case LogicOp::And:            return D3D12_LOGIC_OP_AND;
        case LogicOp::Nand:           return D3D12_LOGIC_OP_NAND;
        case LogicOp::Or:             return D3D12_LOGIC_OP_OR;
        case LogicOp::Nor:            return D3D12_LOGIC_OP_NOR;
        case LogicOp::Xor:            return D3D12_LOGIC_OP_XOR;
        case LogicOp::Equiv:          return D3D12_LOGIC_OP_EQUIV;
        case LogicOp::AndReverse:     return D3D12_LOGIC_OP_AND_REVERSE;
        case LogicOp::AndInverted:    return D3D12_LOGIC_OP_AND_INVERTED;
        case LogicOp::OrReverse:      return D3D12_LOGIC_OP_OR_REVERSE;
        case LogicOp::OrInverted:     return D3D12_LOGIC_OP_OR_INVERTED;
        }

        return invalid_obj<D3D12_LOGIC_OP>();
      }
      D3D12_DEPTH_STENCIL_DESC to_dx12(const DepthStencilDesc& depthStencilState)
      {
        D3D12_DEPTH_STENCIL_DESC desc{};

        desc.DepthEnable = depthStencilState.depth_enable;
        desc.DepthWriteMask = to_dx12(depthStencilState.depth_write_mask);
        desc.DepthFunc = to_dx12(depthStencilState.depth_func);
        desc.StencilEnable = depthStencilState.stencil_enable;
        desc.StencilReadMask = depthStencilState.stencil_read_mask;
        desc.StencilWriteMask = depthStencilState.stencil_write_mask;
        desc.FrontFace = to_dx12(depthStencilState.front_face);
        desc.BackFace = to_dx12(depthStencilState.back_face);

        return desc;
      }
      D3D12_DEPTH_WRITE_MASK to_dx12(DepthWriteMask mask)
      {
        switch (mask)
        {
        case DepthWriteMask::DepthWriteMaskZero: return D3D12_DEPTH_WRITE_MASK_ZERO;
        case DepthWriteMask::DepthWriteMaskAll: return D3D12_DEPTH_WRITE_MASK_ALL;
        }

        return invalid_obj<D3D12_DEPTH_WRITE_MASK>();
      }
      D3D12_DEPTH_STENCILOP_DESC to_dx12(const DepthStencilOpDesc& depthStencilOp)
      {
        D3D12_DEPTH_STENCILOP_DESC desc{};

        desc.StencilDepthFailOp = to_dx12(depthStencilOp.stencil_depth_fail_op);
        desc.StencilPassOp = to_dx12(depthStencilOp.stencil_pass_op);
        desc.StencilFailOp = to_dx12(depthStencilOp.stencil_fail_op);
        desc.StencilFunc = to_dx12(depthStencilOp.stencil_func);

        return desc;
      }
      D3D12_STENCIL_OP to_dx12(StencilOp stencilOp)
      {
        switch (stencilOp)
        {
        case StencilOp::Keep:     return D3D12_STENCIL_OP_KEEP;
        case StencilOp::Zero:     return D3D12_STENCIL_OP_ZERO;
        case StencilOp::Replace:  return D3D12_STENCIL_OP_REPLACE;
        case StencilOp::IncrSat:  return D3D12_STENCIL_OP_INCR_SAT;
        case StencilOp::DecrSat:  return D3D12_STENCIL_OP_DECR_SAT;
        case StencilOp::Invert:   return D3D12_STENCIL_OP_INVERT;
        case StencilOp::Incr:     return D3D12_STENCIL_OP_INCR;
        case StencilOp::Decr:     return D3D12_STENCIL_OP_DECR;
        }

        return invalid_obj<D3D12_STENCIL_OP>();
      }

      D3D12_RESOURCE_STATES to_dx12(ResourceState state)
      {
        switch (state)
        {
        case ResourceState::Common:                                  return D3D12_RESOURCE_STATE_COMMON;
        case ResourceState::VertexAndConstantBuffer:                 return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
        case ResourceState::IndexBuffer:                             return D3D12_RESOURCE_STATE_INDEX_BUFFER;
        case ResourceState::RenderTarget:                            return D3D12_RESOURCE_STATE_RENDER_TARGET;
        case ResourceState::UnorderedAccess:                         return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        case ResourceState::DepthWrite:                              return D3D12_RESOURCE_STATE_DEPTH_WRITE;
        case ResourceState::DepthRead:                               return D3D12_RESOURCE_STATE_DEPTH_READ;
        case ResourceState::NonPixelShaderResource:                  return D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
        case ResourceState::PixelShaderResource:                     return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        case ResourceState::StreamOut:                               return D3D12_RESOURCE_STATE_STREAM_OUT;
        case ResourceState::IndirectArgument:                        return D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
        case ResourceState::CopyDest:                                return D3D12_RESOURCE_STATE_COPY_DEST;
        case ResourceState::CopySource:                              return D3D12_RESOURCE_STATE_COPY_SOURCE;
        case ResourceState::ResolveDest:                             return D3D12_RESOURCE_STATE_RESOLVE_DEST;
        case ResourceState::ResolveSource:                           return D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
        case ResourceState::RaytracingAccelerationStructure:         return D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
        case ResourceState::ShadingRateSource:                       return D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE;
        case ResourceState::GenericRead:                             return D3D12_RESOURCE_STATE_GENERIC_READ;
          //case ResourceState::Present:                                 return D3D12_RESOURCE_STATE_PRESENT                                  ;
        case ResourceState::Predication:                             return D3D12_RESOURCE_STATE_PREDICATION;
        case ResourceState::VideoDecodeRead:                         return D3D12_RESOURCE_STATE_VIDEO_DECODE_READ;
        case ResourceState::VideoDecodeWrite:                        return D3D12_RESOURCE_STATE_VIDEO_DECODE_WRITE;
        case ResourceState::VideoProcessRead:                        return D3D12_RESOURCE_STATE_VIDEO_PROCESS_READ;
        case ResourceState::VideoProcessWrite:                       return D3D12_RESOURCE_STATE_VIDEO_PROCESS_WRITE;
        case ResourceState::VideoEncodeRead:                         return D3D12_RESOURCE_STATE_VIDEO_ENCODE_READ;
        case ResourceState::VideoEncodeWrite:                        return D3D12_RESOURCE_STATE_VIDEO_ENCODE_WRITE;
        default:
          break;
        }

        return invalid_obj<D3D12_RESOURCE_STATES>();
      }
      D3D12_COMMAND_LIST_TYPE to_dx12(GraphicsEngineType type)
      {
        switch (type)
        {
        case GraphicsEngineType::Render:   return D3D12_COMMAND_LIST_TYPE_DIRECT;
        case GraphicsEngineType::Copy:     return D3D12_COMMAND_LIST_TYPE_DIRECT;
        case GraphicsEngineType::Compute:  return D3D12_COMMAND_LIST_TYPE_COMPUTE;
        }

        return invalid_obj<D3D12_COMMAND_LIST_TYPE>();
      }
      D3D12_DESCRIPTOR_HEAP_TYPE to_dx12(ViewHeapType type)
      {
        switch (type)
        {
        case ViewHeapType::ConstantBuffer:  return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        case ViewHeapType::RenderTarget:    return D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        case ViewHeapType::DepthStencil:    return D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        case ViewHeapType::Sampler:         return D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
        }

        return invalid_obj<D3D12_DESCRIPTOR_HEAP_TYPE>();
      }
      D3D12_PRIMITIVE_TOPOLOGY_TYPE to_dx12(PrimitiveTopologyType type)
      {
        switch (type)
        {
        case rex::gfx::PrimitiveTopologyType::Undefined:  return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
        case rex::gfx::PrimitiveTopologyType::Point:      return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
        case rex::gfx::PrimitiveTopologyType::Line:       return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
        case rex::gfx::PrimitiveTopologyType::Triangle:   return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        case rex::gfx::PrimitiveTopologyType::Patch:      return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
        }

        return invalid_obj<D3D12_PRIMITIVE_TOPOLOGY_TYPE>();
      }
      D3D12_SAMPLER_DESC to_dx12(const SamplerDesc& desc)
      {
        D3D12_SAMPLER_DESC sampler_desc{};
        sampler_desc.Filter = d3d::to_dx12(desc.filtering);
        sampler_desc.AddressU = d3d::to_dx12(desc.address_mode_u);
        sampler_desc.AddressV = d3d::to_dx12(desc.address_mode_v);
        sampler_desc.AddressW = d3d::to_dx12(desc.address_mode_w);
        sampler_desc.MipLODBias = desc.mip_lod_bias;
        sampler_desc.MaxAnisotropy = desc.max_anisotropy;
        sampler_desc.ComparisonFunc = d3d::to_dx12(desc.comparison_func);
        //sampler_desc.BorderColor = d3d::to_dx12(desc.border_color);
        sampler_desc.MinLOD = desc.min_lod;
        sampler_desc.MaxLOD = desc.max_lod;
        //sampler_desc.ShaderRegister = desc.shader_register;
        //sampler_desc.RegisterSpace = desc.register_space;
        //sampler_desc.ShaderVisibility = d3d::to_dx12(desc.shader_visibility);

        return sampler_desc;
      }
      D3D12_DESCRIPTOR_RANGE_TYPE to_dx12(ShaderParameterType type)
      {
        switch (type)
        {
        case rex::gfx::ShaderParameterType::ConstantBuffer:    return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
        case rex::gfx::ShaderParameterType::Texture:           return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        case rex::gfx::ShaderParameterType::Sampler:           return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
        default: break;
        }

        return invalid_obj<D3D12_DESCRIPTOR_RANGE_TYPE>();
      }



      // ------------------------------------
      // Converts from generic REX classes -> DirectX REX classes
      // ------------------------------------
      DxCommandQueue* to_dx12(CommandQueue* cmdQueue)
      {
        return static_cast<DxCommandQueue*>(cmdQueue);
      }
      DxCommandAllocator* to_dx12(CommandAllocator* cmdAlloc)
      {
        return static_cast<DxCommandAllocator*>(cmdAlloc);
      }
      DxRootSignature* to_dx12(RootSignature* rootSig)
      {
        return static_cast<DxRootSignature*>(rootSig);
      }
      DxShader* to_dx12(Shader* shader)
      {
        return static_cast<DxShader*>(shader);
      }
      DxTexture2D* to_dx12(Texture2D* texture)
      {
        return static_cast<DxTexture2D*>(texture);
      }
      DxInputLayout* to_dx12(InputLayout* inputLayout)
      {
        return static_cast<DxInputLayout*>(inputLayout);
      }
      DxViewHeap* to_dx12(ViewHeap* descHeap)
      {
        return static_cast<DxViewHeap*>(descHeap);
      }
      DxSampler2D* to_dx12(Sampler2D* sampler)
      {
        return static_cast<DxSampler2D*>(sampler);
      }
      DxResourceView* to_dx12(ResourceView* resourceView)
      {
        return static_cast<DxResourceView*>(resourceView);
      }
      const DxResourceView* to_dx12(const ResourceView* resourceView)
      {
        return static_cast<const DxResourceView*>(resourceView);
      }
      DxFence* to_dx12(Fence* fence)
      {
        return static_cast<DxFence*>(fence);
      }
      DxConstantBuffer* to_dx12(ConstantBuffer* constantBuffer)
      {
        return static_cast<DxConstantBuffer*>(constantBuffer);
      }
      DxVertexBuffer* to_dx12(VertexBuffer* vertexBuffer)
      {
        return static_cast<DxVertexBuffer*>(vertexBuffer);
      }
      DxIndexBuffer* to_dx12(IndexBuffer* indexBuffer)
      {
        return static_cast<DxIndexBuffer*>(indexBuffer);
      }
      DxUploadBuffer* to_dx12(UploadBuffer* uploadBuffer)
      {
        return static_cast<DxUploadBuffer*>(uploadBuffer);
      }
      DxRenderTarget* to_dx12(RenderTarget* renderTarget)
      {
        return static_cast<DxRenderTarget*>(renderTarget);
      }
      DxDepthStencilBuffer* to_dx12(DepthStencilBuffer* depthStencilBuffer)
      {
        return static_cast<DxDepthStencilBuffer*>(depthStencilBuffer);
      }
      DxPipelineState* to_dx12(PipelineState* pso)
      {
        return static_cast<DxPipelineState*>(pso);
      }
      DxShaderPipelineParameters to_dx12(const rsl::vector<ShaderParameterDeclaration>& parameters)
      {
        DxShaderPipelineParameters dx_params{};

        dx_params.root_parameters.reserve(parameters.size());
        for (const ShaderParameterDeclaration& param : parameters)
        {
          // Constant buffers are expected to be bound inline
          if (param.type == ShaderParameterType::ConstantBuffer)
          {
            REX_ASSERT_X(param.ranges.size() == 1, "Constant buffer are expected to be bound independantly and not in ranges");
            const ViewRangeDeclaration& view_range = param.ranges.front();
            dx_params.root_parameters.emplace_back().InitAsConstantBufferView(view_range.base_register, view_range.register_space, to_dx12(param.visibility));
          }
          // All other type of parameters are expected to be tied within ranges
          else
          {
            rsl::vector<D3D12_DESCRIPTOR_RANGE>& desc_ranges = dx_params.ranges.emplace_back();
            for (const auto& view_range : param.ranges)
            {
              D3D12_DESCRIPTOR_RANGE& dx_range = desc_ranges.emplace_back();
              dx_range.BaseShaderRegister = view_range.base_register;
              dx_range.NumDescriptors = view_range.num_views;
              dx_range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // We pack all our view tables together, so we can just follow from where we left of
              dx_range.RangeType = to_dx12(view_range.type);
              dx_range.RegisterSpace = view_range.register_space;
            }
            dx_params.root_parameters.emplace_back().InitAsDescriptorTable(desc_ranges.size(), desc_ranges.data(), to_dx12(param.visibility));
          }
        }
       
        return dx_params;
      }

      const DxShader* to_dx12(const Shader* shader)
      {
        return static_cast<const DxShader*>(shader);
      }

      // ------------------------------------
      // Returned the wrapped dx12 resource of a class
      // ------------------------------------
      ID3D12Resource* dx12_resource(ConstantBuffer* buffer)
      {
        return static_cast<DxConstantBuffer*>(buffer)->dx_object();
      }
      ID3D12Resource* dx12_resource(VertexBuffer* buffer)
      {
        return static_cast<DxVertexBuffer*>(buffer)->dx_object();
      }
      ID3D12Resource* dx12_resource(IndexBuffer* buffer)
      {
        return static_cast<DxIndexBuffer*>(buffer)->dx_object();
      }
      ID3D12Resource* dx12_resource(UploadBuffer* buffer)
      {
        return static_cast<DxUploadBuffer*>(buffer)->dx_object();
      }
      ID3D12Resource* dx12_resource(Texture2D* texture)
      {
        return static_cast<DxTexture2D*>(texture)->dx_object();
      }
      ID3D12PipelineState* dx12_pso(PipelineState* pso)
      {
        if (!pso)
        {
          return nullptr;
        }

        return static_cast<DxPipelineState*>(pso)->dx_object();
      }


      // ------------------------------------
      // Return from Directx -> REX
      // ------------------------------------
      ShaderArithmeticType from_dx12_shader_arithmetic_type(DXGI_FORMAT format)
      {
        switch (format)
        {
          // 4 component types
        case DXGI_FORMAT_R32G32B32A32_FLOAT:      return ShaderArithmeticType::Float4;

        case DXGI_FORMAT_R32G32B32A32_UINT:       return ShaderArithmeticType::Uint4;

        case DXGI_FORMAT_R32G32B32A32_SINT:       return ShaderArithmeticType::Int4;
        
        case DXGI_FORMAT_R16G16B16A16_FLOAT:      return ShaderArithmeticType::Half4;
        
        case DXGI_FORMAT_R16G16B16A16_UNORM:      return ShaderArithmeticType::Ushort4Norm;
        case DXGI_FORMAT_R16G16B16A16_UINT:       return ShaderArithmeticType::Ushort4;
        case DXGI_FORMAT_R16G16B16A16_SNORM:      return ShaderArithmeticType::Short4Norm;
        case DXGI_FORMAT_R16G16B16A16_SINT:       return ShaderArithmeticType::Short4;
        
        case DXGI_FORMAT_R8G8B8A8_UNORM:          return ShaderArithmeticType::UChar4Norm;
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:     return ShaderArithmeticType::UChar4Norm;
        case DXGI_FORMAT_R8G8B8A8_UINT:           return ShaderArithmeticType::UChar4;
        case DXGI_FORMAT_R8G8B8A8_SNORM:          return ShaderArithmeticType::Char4Norm;
        case DXGI_FORMAT_R8G8B8A8_SINT:           return ShaderArithmeticType::Char4;
        case DXGI_FORMAT_B8G8R8A8_UNORM:          return ShaderArithmeticType::UChar4Norm;
        case DXGI_FORMAT_B8G8R8X8_UNORM:          return ShaderArithmeticType::UChar4Norm;
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:     return ShaderArithmeticType::UChar4Norm;
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:     return ShaderArithmeticType::UChar4Norm;

          // 3 component types
        case DXGI_FORMAT_R32G32B32_FLOAT:         return ShaderArithmeticType::Float3;
        case DXGI_FORMAT_R32G32B32_UINT:          return ShaderArithmeticType::Uint3;
        case DXGI_FORMAT_R32G32B32_SINT:          return ShaderArithmeticType::Int3;

          // 2 component types
        case DXGI_FORMAT_R32G32_FLOAT:            return ShaderArithmeticType::Float2;

        case DXGI_FORMAT_R32G32_UINT:             return ShaderArithmeticType::Uint2;
        case DXGI_FORMAT_R32G32_SINT:             return ShaderArithmeticType::Int2;

        case DXGI_FORMAT_R16G16_FLOAT:            return ShaderArithmeticType::Half2;
        case DXGI_FORMAT_R16G16_UNORM:            return ShaderArithmeticType::Ushort2Norm;
        case DXGI_FORMAT_R16G16_UINT:             return ShaderArithmeticType::Ushort2;
        case DXGI_FORMAT_R16G16_SNORM:            return ShaderArithmeticType::Short2Norm;
        case DXGI_FORMAT_R16G16_SINT:             return ShaderArithmeticType::Short2;

        case DXGI_FORMAT_R8G8_UNORM:              return ShaderArithmeticType::UChar2Norm;
        case DXGI_FORMAT_R8G8_UINT:               return ShaderArithmeticType::UChar2;
        case DXGI_FORMAT_R8G8_SNORM:              return ShaderArithmeticType::Char2Norm;
        case DXGI_FORMAT_R8G8_SINT:               return ShaderArithmeticType::Char2;

          // 1 component types
        case DXGI_FORMAT_D32_FLOAT:               return ShaderArithmeticType::Float;
        case DXGI_FORMAT_R32_FLOAT:               return ShaderArithmeticType::Float;
        case DXGI_FORMAT_R32_UINT:                return ShaderArithmeticType::Uint;
        case DXGI_FORMAT_R32_SINT:                return ShaderArithmeticType::Int;

        case DXGI_FORMAT_R16_FLOAT:               return ShaderArithmeticType::Half;
        case DXGI_FORMAT_D16_UNORM:               return ShaderArithmeticType::UshortNorm;
        case DXGI_FORMAT_R16_UNORM:               return ShaderArithmeticType::UshortNorm;
        case DXGI_FORMAT_R16_UINT:                return ShaderArithmeticType::Ushort;
        case DXGI_FORMAT_R16_SNORM:               return ShaderArithmeticType::ShortNorm;
        case DXGI_FORMAT_R16_SINT:                return ShaderArithmeticType::Short;

        case DXGI_FORMAT_R8_UNORM:                return ShaderArithmeticType::UCharNorm;
        case DXGI_FORMAT_R8_UINT:                 return ShaderArithmeticType::UChar;
        case DXGI_FORMAT_R8_SNORM:                return ShaderArithmeticType::CharNorm;
        case DXGI_FORMAT_R8_SINT:                 return ShaderArithmeticType::Char;
        case DXGI_FORMAT_A8_UNORM:                return ShaderArithmeticType::CharNorm;
        case DXGI_FORMAT_R1_UNORM:                return ShaderArithmeticType::Bool;

        default: break;
        }

        return invalid_obj<ShaderArithmeticType>();
      }

      TextureFormat from_dx12(DXGI_FORMAT type)
      {
        switch (type)
        {
        case DXGI_FORMAT_UNKNOWN: return TextureFormat::Unknown;

        case DXGI_FORMAT_R8G8B8A8_UNORM:        return TextureFormat::Unorm4;
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:   return TextureFormat::Unorm4Srgb;

        case DXGI_FORMAT_R32G32B32A32_TYPELESS:
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
        case DXGI_FORMAT_R32G32B32A32_UINT:
        case DXGI_FORMAT_R32G32B32A32_SINT:
        case DXGI_FORMAT_R32G32B32_TYPELESS:
        case DXGI_FORMAT_R32G32B32_FLOAT:
        case DXGI_FORMAT_R32G32B32_UINT:
        case DXGI_FORMAT_R32G32B32_SINT:
        case DXGI_FORMAT_R16G16B16A16_TYPELESS:
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
        case DXGI_FORMAT_R16G16B16A16_UNORM:
        case DXGI_FORMAT_R16G16B16A16_UINT:
        case DXGI_FORMAT_R16G16B16A16_SNORM:
        case DXGI_FORMAT_R16G16B16A16_SINT:
        case DXGI_FORMAT_R32G32_TYPELESS:
        case DXGI_FORMAT_R32G32_FLOAT:
        case DXGI_FORMAT_R32G32_UINT:
        case DXGI_FORMAT_R32G32_SINT:
        case DXGI_FORMAT_R32G8X24_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
        case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
        case DXGI_FORMAT_R10G10B10A2_TYPELESS:
        case DXGI_FORMAT_R10G10B10A2_UNORM:
        case DXGI_FORMAT_R10G10B10A2_UINT:
        case DXGI_FORMAT_R11G11B10_FLOAT:
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        case DXGI_FORMAT_R8G8B8A8_UINT:
        case DXGI_FORMAT_R8G8B8A8_SNORM:
        case DXGI_FORMAT_R8G8B8A8_SINT:
        case DXGI_FORMAT_R16G16_TYPELESS:
        case DXGI_FORMAT_R16G16_FLOAT:
        case DXGI_FORMAT_R16G16_UNORM:
        case DXGI_FORMAT_R16G16_UINT:
        case DXGI_FORMAT_R16G16_SNORM:
        case DXGI_FORMAT_R16G16_SINT:
        case DXGI_FORMAT_R32_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT:
        case DXGI_FORMAT_R32_FLOAT:
        case DXGI_FORMAT_R32_UINT:
        case DXGI_FORMAT_R32_SINT:
        case DXGI_FORMAT_R24G8_TYPELESS:
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
        case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
        case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
        case DXGI_FORMAT_R8G8_TYPELESS:
        case DXGI_FORMAT_R8G8_UNORM:
        case DXGI_FORMAT_R8G8_UINT:
        case DXGI_FORMAT_R8G8_SNORM:
        case DXGI_FORMAT_R8G8_SINT:
        case DXGI_FORMAT_R16_TYPELESS:
        case DXGI_FORMAT_R16_FLOAT:
        case DXGI_FORMAT_D16_UNORM:
        case DXGI_FORMAT_R16_UNORM:
        case DXGI_FORMAT_R16_UINT:
        case DXGI_FORMAT_R16_SNORM:
        case DXGI_FORMAT_R16_SINT:
        case DXGI_FORMAT_R8_TYPELESS:
        case DXGI_FORMAT_R8_UNORM:
        case DXGI_FORMAT_R8_UINT:
        case DXGI_FORMAT_R8_SNORM:
        case DXGI_FORMAT_R8_SINT:
        case DXGI_FORMAT_A8_UNORM:
        case DXGI_FORMAT_R1_UNORM:
        case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
        case DXGI_FORMAT_R8G8_B8G8_UNORM:
        case DXGI_FORMAT_G8R8_G8B8_UNORM:
        case DXGI_FORMAT_BC1_TYPELESS:
        case DXGI_FORMAT_BC1_UNORM:
        case DXGI_FORMAT_BC1_UNORM_SRGB:
        case DXGI_FORMAT_BC2_TYPELESS:
        case DXGI_FORMAT_BC2_UNORM:
        case DXGI_FORMAT_BC2_UNORM_SRGB:
        case DXGI_FORMAT_BC3_TYPELESS:
        case DXGI_FORMAT_BC3_UNORM:
        case DXGI_FORMAT_BC3_UNORM_SRGB:
        case DXGI_FORMAT_BC4_TYPELESS:
        case DXGI_FORMAT_BC4_UNORM:
        case DXGI_FORMAT_BC4_SNORM:
        case DXGI_FORMAT_BC5_TYPELESS:
        case DXGI_FORMAT_BC5_UNORM:
        case DXGI_FORMAT_BC5_SNORM:
        case DXGI_FORMAT_B5G6R5_UNORM:
        case DXGI_FORMAT_B5G5R5A1_UNORM:
        case DXGI_FORMAT_B8G8R8A8_UNORM:
        case DXGI_FORMAT_B8G8R8X8_UNORM:
        case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
        case DXGI_FORMAT_B8G8R8A8_TYPELESS:
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        case DXGI_FORMAT_B8G8R8X8_TYPELESS:
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        case DXGI_FORMAT_BC6H_TYPELESS:
        case DXGI_FORMAT_BC6H_UF16:
        case DXGI_FORMAT_BC6H_SF16:
        case DXGI_FORMAT_BC7_TYPELESS:
        case DXGI_FORMAT_BC7_UNORM:
        case DXGI_FORMAT_BC7_UNORM_SRGB:
        case DXGI_FORMAT_AYUV:
        case DXGI_FORMAT_Y410:
        case DXGI_FORMAT_Y416:
        case DXGI_FORMAT_NV12:
        case DXGI_FORMAT_P010:
        case DXGI_FORMAT_P016:
        case DXGI_FORMAT_420_OPAQUE:
        case DXGI_FORMAT_YUY2:
        case DXGI_FORMAT_Y210:
        case DXGI_FORMAT_Y216:
        case DXGI_FORMAT_NV11:
        case DXGI_FORMAT_AI44:
        case DXGI_FORMAT_IA44:
        case DXGI_FORMAT_P8:
        case DXGI_FORMAT_A8P8:
        case DXGI_FORMAT_B4G4R4A4_UNORM:
        case DXGI_FORMAT_P208:
        case DXGI_FORMAT_V208:
        case DXGI_FORMAT_V408:
        case DXGI_FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE:
        case DXGI_FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE:
        case DXGI_FORMAT_FORCE_UINT:
        default:
          REX_ASSERT("Unknown DX12 texture format, cannot convert to rex texture format");
        }

        return invalid_obj<TextureFormat>();
      }
			ShaderClassType from_dx12(D3D_SHADER_VARIABLE_CLASS type)
			{
				switch (type)
				{
				case D3D_SVC_SCALAR:            return ShaderClassType::Scalar;
				case D3D_SVC_VECTOR:            return ShaderClassType::Vector;
				case D3D_SVC_MATRIX_ROWS:       return ShaderClassType::MatrixRows;
				case D3D_SVC_MATRIX_COLUMNS:    return ShaderClassType::MatrixColumns;
				case D3D_SVC_OBJECT:            return ShaderClassType::Object;
				case D3D_SVC_STRUCT:            return ShaderClassType::Struct;
				case D3D_SVC_INTERFACE_CLASS:   return ShaderClassType::InterfaceClass;
				case D3D_SVC_INTERFACE_POINTER: return ShaderClassType::InterfacePointer;
				default:
					REX_ASSERT("Unknown DX12 shader class type, cannot convert to rex shader class type");
				}

        return invalid_obj<ShaderClassType>();
			}
			ShaderVariableType from_dx12(D3D_SHADER_VARIABLE_TYPE type)
			{
				switch (type)
				{
				case D3D_SVT_VOID:                                    return ShaderVariableType::Void;
				case D3D_SVT_BOOL:                                    return ShaderVariableType::Bool;
				case D3D_SVT_INT:                                     return ShaderVariableType::Int;
				case D3D_SVT_FLOAT:                                   return ShaderVariableType::Float;
				case D3D_SVT_STRING:                                  return ShaderVariableType::String;
				case D3D_SVT_TEXTURE:                                 return ShaderVariableType::Texture;
				case D3D_SVT_TEXTURE1D:                               return ShaderVariableType::Texture1d;
				case D3D_SVT_TEXTURE2D:                               return ShaderVariableType::Texture2d;
				case D3D_SVT_TEXTURE3D:                               return ShaderVariableType::Texture3d;
				case D3D_SVT_TEXTURECUBE:                             return ShaderVariableType::TextureCube;
				case D3D_SVT_SAMPLER:                                 return ShaderVariableType::Sampler;
				case D3D_SVT_SAMPLER1D:                               return ShaderVariableType::Sampler1d;
				case D3D_SVT_SAMPLER2D:                               return ShaderVariableType::Sampler2d;
				case D3D_SVT_SAMPLER3D:                               return ShaderVariableType::Sampler3d;
				case D3D_SVT_SAMPLERCUBE:                             return ShaderVariableType::SamplerCube;
				case D3D_SVT_PIXELSHADER:                             return ShaderVariableType::PixelShader;
				case D3D_SVT_VERTEXSHADER:                            return ShaderVariableType::VertexShader;
				case D3D_SVT_PIXELFRAGMENT:                           return ShaderVariableType::PixelFragment;
				case D3D_SVT_VERTEXFRAGMENT:                          return ShaderVariableType::VertexFragment;
				case D3D_SVT_UINT:                                    return ShaderVariableType::Uint;
				case D3D_SVT_UINT8:                                   return ShaderVariableType::Uint8;
				case D3D_SVT_GEOMETRYSHADER:                          return ShaderVariableType::GeometryShader;
				case D3D_SVT_RASTERIZER:                              return ShaderVariableType::Rasterizer;
				case D3D_SVT_DEPTHSTENCIL:                            return ShaderVariableType::DepthStencil;
				case D3D_SVT_BLEND:                                   return ShaderVariableType::Blend;
				case D3D_SVT_BUFFER:                                  return ShaderVariableType::Buffer;
				case D3D_SVT_CBUFFER:                                 return ShaderVariableType::CBuffer;
				case D3D_SVT_TBUFFER:                                 return ShaderVariableType::TBuffer;
				case D3D_SVT_TEXTURE1DARRAY:                          return ShaderVariableType::Texture1dArray;
				case D3D_SVT_TEXTURE2DARRAY:                          return ShaderVariableType::Texture2dArray;
				case D3D_SVT_RENDERTARGETVIEW:                        return ShaderVariableType::RenderTargetView;
				case D3D_SVT_DEPTHSTENCILVIEW:                        return ShaderVariableType::DepthStencilView;
				case D3D_SVT_TEXTURE2DMS:                             return ShaderVariableType::Texture2dms;
				case D3D_SVT_TEXTURE2DMSARRAY:                        return ShaderVariableType::Texture2dmsArray;
				case D3D_SVT_TEXTURECUBEARRAY:                        return ShaderVariableType::TextureCubeArray;
				case D3D_SVT_HULLSHADER:                              return ShaderVariableType::HullShader;
				case D3D_SVT_DOMAINSHADER:                            return ShaderVariableType::DomainShader;
				case D3D_SVT_INTERFACE_POINTER:                       return ShaderVariableType::InterfacePointer;
				case D3D_SVT_COMPUTESHADER:                           return ShaderVariableType::ComputeShader;
				case D3D_SVT_DOUBLE:                                  return ShaderVariableType::Double;
				case D3D_SVT_RWTEXTURE1D:                             return ShaderVariableType::Rwtexture1d;
				case D3D_SVT_RWTEXTURE1DARRAY:                        return ShaderVariableType::Rwtexture1dArray;
				case D3D_SVT_RWTEXTURE2D:                             return ShaderVariableType::Rwtexture2d;
				case D3D_SVT_RWTEXTURE2DARRAY:                        return ShaderVariableType::Rwtexture2dArray;
				case D3D_SVT_RWTEXTURE3D:                             return ShaderVariableType::Rwtexture3d;
				case D3D_SVT_RWBUFFER:                                return ShaderVariableType::RwBuffer;
				case D3D_SVT_BYTEADDRESS_BUFFER:                      return ShaderVariableType::ByteAddressBuffer;
				case D3D_SVT_RWBYTEADDRESS_BUFFER:                    return ShaderVariableType::RwByteAddressBuffer;
				case D3D_SVT_STRUCTURED_BUFFER:                       return ShaderVariableType::StructuredBuffer;
				case D3D_SVT_RWSTRUCTURED_BUFFER:                     return ShaderVariableType::RwstructuredBuffer;
				case D3D_SVT_APPEND_STRUCTURED_BUFFER:                return ShaderVariableType::AppendStructuredBuffer;
				case D3D_SVT_CONSUME_STRUCTURED_BUFFER:               return ShaderVariableType::ConsumeStructuredBuffer;
				case D3D_SVT_MIN8FLOAT:                               return ShaderVariableType::Min8float;
				case D3D_SVT_MIN10FLOAT:                              return ShaderVariableType::Min10float;
				case D3D_SVT_MIN16FLOAT:                              return ShaderVariableType::Min16float;
				case D3D_SVT_MIN12INT:                                return ShaderVariableType::Min12int;
				case D3D_SVT_MIN16INT:                                return ShaderVariableType::Min16int;
				case D3D_SVT_MIN16UINT:                               return ShaderVariableType::Min16uint;
				default:
					REX_ASSERT("Unknown DX12 shader variable type, cannot convert to rex shader variable type");
				}

        return invalid_obj<ShaderVariableType>();
			}

      ShaderVisibility from_dx12(D3D12_SHADER_VISIBILITY visibility)
      {
        switch (visibility)
        {
        case D3D12_SHADER_VISIBILITY_ALL:                   return ShaderVisibility::All;
        case D3D12_SHADER_VISIBILITY_VERTEX:                return ShaderVisibility::Vertex;
        case D3D12_SHADER_VISIBILITY_HULL:                  return ShaderVisibility::Hull;
        case D3D12_SHADER_VISIBILITY_DOMAIN:                return ShaderVisibility::Domain;
        case D3D12_SHADER_VISIBILITY_GEOMETRY:              return ShaderVisibility::Geometry;
        case D3D12_SHADER_VISIBILITY_PIXEL:                 return ShaderVisibility::Pixel;
        case D3D12_SHADER_VISIBILITY_AMPLIFICATION:         return ShaderVisibility::Amplification;
        case D3D12_SHADER_VISIBILITY_MESH:                  return ShaderVisibility::Mesh;
        }

        return invalid_obj<ShaderVisibility>();
      }
    }
  }
} // namespace rex