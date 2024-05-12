#include "rex_directx/utility/dx_util.h"
#include "rex_directx/utility/d3dx12.h"
#include "rex_directx/diagnostics/log.h"

#include "rex_renderer_core/rendering/index_buffer_format.h"
#include "rex_engine/memory/pointer_math.h"
#include "rex_engine/engine/invalid_object.h"

#include "rex_std/bonus/utility.h"

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

    s32 total_texture_size(s32 width, s32 height, renderer::TextureFormat format)
    {
      const s32 format_size = rex::d3d::format_byte_size(format);
      const s32 alignment = D3D12_TEXTURE_DATA_PITCH_ALIGNMENT;
      s32 pitch_size = width * format_size;
      pitch_size = align(pitch_size, alignment);

      return pitch_size * height;
    }

    //-------------------------------------------------------------------------
    D3D12_FILL_MODE to_dx12(renderer::FillMode mode)
    {
      switch (mode)
      {
      case renderer::FillMode::Solid: return D3D12_FILL_MODE_SOLID;
      case renderer::FillMode::Wireframe: return D3D12_FILL_MODE_WIREFRAME;
      default: break;
      }

      REX_ASSERT("Unsupported fill mode given");
      return D3D12_FILL_MODE_SOLID;
    }
    //-------------------------------------------------------------------------
    D3D12_CULL_MODE to_dx12(renderer::CullMode mode)
    {
      switch (mode)
      {
      case renderer::CullMode::None: return D3D12_CULL_MODE_NONE;
      case renderer::CullMode::Front: return D3D12_CULL_MODE_FRONT;
      case renderer::CullMode::Back: return D3D12_CULL_MODE_BACK;
      default: break;
      }

      REX_ASSERT("Unsupported cull mode given");
      return D3D12_CULL_MODE_NONE;
    }
    //-------------------------------------------------------------------------
    DXGI_FORMAT to_dx12(renderer::VertexBufferFormat format)
    {
      switch (format)
      {
      case renderer::VertexBufferFormat::Float1: return DXGI_FORMAT_R32_FLOAT;
      case renderer::VertexBufferFormat::Float2: return DXGI_FORMAT_R32G32_FLOAT;
      case renderer::VertexBufferFormat::Float3: return DXGI_FORMAT_R32G32B32_FLOAT;
      case renderer::VertexBufferFormat::Float4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
      case renderer::VertexBufferFormat::UNorm1: return DXGI_FORMAT_R8_UNORM;
      case renderer::VertexBufferFormat::UNorm2: return DXGI_FORMAT_R8G8_UNORM;
      case renderer::VertexBufferFormat::UNorm4: return DXGI_FORMAT_R8G8B8A8_UNORM;
      default: break;
      }
      REX_ASSERT("Unsupported vertex buffer format given");
      return DXGI_FORMAT_UNKNOWN;
    }
    //-------------------------------------------------------------------------
    DXGI_FORMAT to_dx12(renderer::IndexBufferFormat format)
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
    DXGI_FORMAT to_dx12(renderer::TextureFormat format)
    {
      switch (format)
      {
      case renderer::TextureFormat::Unorm4Srgb: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
      case renderer::TextureFormat::Unorm4: return DXGI_FORMAT_R8G8B8A8_UNORM;
      default: break;
      }
      REX_ASSERT("Unsupported vertex buffer format given");
      return DXGI_FORMAT_UNKNOWN;
    }

    //-------------------------------------------------------------------------
    D3D12_PRIMITIVE_TOPOLOGY to_dx12(renderer::PrimitiveTopology topology)
    {
      switch (topology)
      {
      case renderer::PrimitiveTopology::LineList: return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
      case renderer::PrimitiveTopology::LineStrip: return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
      case renderer::PrimitiveTopology::PointList: return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
      case renderer::PrimitiveTopology::TriangleList: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
      case renderer::PrimitiveTopology::TriangleStrip: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
      default: break;
      }
      REX_ASSERT("Unsupported primitive topology given");
      return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    }
    //-------------------------------------------------------------------------
    D3D12_INPUT_CLASSIFICATION to_dx12(renderer::InputLayoutClassification classification)
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
    s32 format_byte_size(renderer::TextureFormat format)
    {
      DXGI_FORMAT d3d_format = to_dx12(format);
      return format_byte_size(d3d_format);
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
    s32 texture_2d_size(DXGI_FORMAT format, s32 width, s32 height)
    {
      s32 format_size = d3d::format_byte_size(format);
      s32 upload_pitch = align(width * format_size, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
      s32 total_size = height * upload_pitch;
      return total_size;
    }


    D3D12_SHADER_VISIBILITY to_dx12(renderer::ShaderVisibility visibility)
    {
      switch (visibility)
      {
      case rex::renderer::ShaderVisibility::Vertex: return D3D12_SHADER_VISIBILITY_VERTEX;
      case rex::renderer::ShaderVisibility::Pixel: return D3D12_SHADER_VISIBILITY_PIXEL;
      case rex::renderer::ShaderVisibility::All: return D3D12_SHADER_VISIBILITY_ALL;
      }

      REX_ASSERT("Unsupported shader visibility for directx 12: {}", rsl::enum_refl::enum_name(visibility));
      return invalid_obj<D3D12_SHADER_VISIBILITY>();
    }
    D3D12_FILTER to_dx12(renderer::SamplerFiltering filter)
    {
      switch (filter)
      {
      case rex::renderer::SamplerFiltering::MinMagMipPoint:                                      return D3D12_FILTER_MIN_MAG_MIP_POINT;
      case rex::renderer::SamplerFiltering::MinMagPointMipLinear:                                return D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
      case rex::renderer::SamplerFiltering::MinPointMagLinearMipPoint:                           return D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
      case rex::renderer::SamplerFiltering::MinPointMagMipLinear:                                return D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR;
      case rex::renderer::SamplerFiltering::MinLinearMagMipPoint:                                return D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;
      case rex::renderer::SamplerFiltering::MinLinearMagPointMipLinear:                          return D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
      case rex::renderer::SamplerFiltering::MinMagLinearMipPoint:                                return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
      case rex::renderer::SamplerFiltering::MinMagMipLinear:                                     return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
      case rex::renderer::SamplerFiltering::Anisotropic:                                         return D3D12_FILTER_ANISOTROPIC;
      case rex::renderer::SamplerFiltering::ComparisonMinMagMipPoint:                            return D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
      case rex::renderer::SamplerFiltering::ComparisonMinMagPointMipLinear:                      return D3D12_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
      case rex::renderer::SamplerFiltering::ComparisonMinPointMagLinearMipPoint:                 return D3D12_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
      case rex::renderer::SamplerFiltering::ComparisonMinPointMagMipLinear:                      return D3D12_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
      case rex::renderer::SamplerFiltering::ComparisonMinLinearMagMipPoint:                      return D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
      case rex::renderer::SamplerFiltering::ComparisonMinLinearMagPointMipLinear:                return D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
      case rex::renderer::SamplerFiltering::ComparisonMinMagLinearMipPoint:                      return D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
      case rex::renderer::SamplerFiltering::ComparisonMinMagMipLinear:                           return D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
      case rex::renderer::SamplerFiltering::ComparisonAnisotropic:                               return D3D12_FILTER_COMPARISON_ANISOTROPIC;
      case rex::renderer::SamplerFiltering::MinimumMinMagMipPoint:                               return D3D12_FILTER_MINIMUM_MIN_MAG_MIP_POINT;
      case rex::renderer::SamplerFiltering::MinimumMinMagPointMipLinear:                         return D3D12_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR;
      case rex::renderer::SamplerFiltering::MinimumMinPointMagLinearMipPoint:                    return D3D12_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
      case rex::renderer::SamplerFiltering::MinimumMinPointMagMipLinear:                         return D3D12_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR;
      case rex::renderer::SamplerFiltering::MinimumMinLinearMagMipPoint:                         return D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT;
      case rex::renderer::SamplerFiltering::MinimumMinLinearMagPointMipLinear:                   return D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
      case rex::renderer::SamplerFiltering::MinimumMinMagLinearMipPoint:                         return D3D12_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT;
      case rex::renderer::SamplerFiltering::MinimumMinMagMipLinear:                              return D3D12_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR;
      case rex::renderer::SamplerFiltering::MinimumAnisotropic:                                  return D3D12_FILTER_MINIMUM_ANISOTROPIC;
      case rex::renderer::SamplerFiltering::MaximumMinMagMipPoint:                               return D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_POINT;
      case rex::renderer::SamplerFiltering::MaximumMinMagPointMipLinear:                         return D3D12_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR;
      case rex::renderer::SamplerFiltering::MaximumMinPointMagLinearMipPoint:                    return D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
      case rex::renderer::SamplerFiltering::MaximumMinPointMagMipLinear:                         return D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR;
      case rex::renderer::SamplerFiltering::MaximumMinLinearMagMipPoint:                         return D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT;
      case rex::renderer::SamplerFiltering::MaximumMinLinearMagPointMipLinear:                   return D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
      case rex::renderer::SamplerFiltering::MaximumMinMagLinearMipPoint:                         return D3D12_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT;
      case rex::renderer::SamplerFiltering::MaximumMinMagMipLinear:                              return D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR;
      case rex::renderer::SamplerFiltering::MaximumAnisotropic:                                  return D3D12_FILTER_MAXIMUM_ANISOTROPIC;
      }

      REX_ASSERT("Unsupported sampler filter for directx 12: {}", rsl::enum_refl::enum_name(filter));
      return invalid_obj<D3D12_FILTER>();
    }
    D3D12_COMPARISON_FUNC to_dx12(renderer::ComparisonFunc comparisonFunc)
    {
      switch (comparisonFunc)
      {
      case rex::renderer::ComparisonFunc::Never: return D3D12_COMPARISON_FUNC_NEVER;
      case rex::renderer::ComparisonFunc::Always: return D3D12_COMPARISON_FUNC_ALWAYS;
      case rex::renderer::ComparisonFunc::Less: return D3D12_COMPARISON_FUNC_LESS;
      case rex::renderer::ComparisonFunc::LessEqual: return D3D12_COMPARISON_FUNC_LESS_EQUAL;
      case rex::renderer::ComparisonFunc::Equal: return D3D12_COMPARISON_FUNC_EQUAL;
      case rex::renderer::ComparisonFunc::Greater: return D3D12_COMPARISON_FUNC_GREATER;
      case rex::renderer::ComparisonFunc::GreaterEqual: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
      case rex::renderer::ComparisonFunc::NotEqual: return D3D12_COMPARISON_FUNC_NOT_EQUAL;
      }

      REX_ASSERT("Unsupported comparison func for directx 12: {}", rsl::enum_refl::enum_name(comparisonFunc));
      return invalid_obj<D3D12_COMPARISON_FUNC>();
    }
    D3D12_STATIC_BORDER_COLOR to_dx12(renderer::BorderColor borderColor)
    {
      switch (borderColor)
      {
      case rex::renderer::BorderColor::TransparentBlack: return D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
      case rex::renderer::BorderColor::OpaqueBlack: return D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
      case rex::renderer::BorderColor::OpaqueWhite: return D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
      }

      REX_ASSERT("Unsupported border color for directx 12: {}", rsl::enum_refl::enum_name(borderColor));
      return invalid_obj<D3D12_STATIC_BORDER_COLOR>();
    }
    D3D12_TEXTURE_ADDRESS_MODE to_dx12(renderer::TextureAddressMode addressMode)
    {
      switch (addressMode)
      {
      case rex::renderer::TextureAddressMode::Wrap: return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
      case rex::renderer::TextureAddressMode::Mirror: return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
      case rex::renderer::TextureAddressMode::Clamp: return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
      case rex::renderer::TextureAddressMode::Border: return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
      case rex::renderer::TextureAddressMode::MirrorOnce: return D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
      }

      REX_ASSERT("Unsupported addressmode for directx 12: {}", rsl::enum_refl::enum_name(addressMode));
      return invalid_obj<D3D12_TEXTURE_ADDRESS_MODE>();
    }
    D3D12_BLEND_DESC to_dx12(const rhi::BlendDesc& blendState)
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
    D3D12_BLEND to_dx12(rhi::Blend blend)
    {
      switch (blend)
      {
      case rex::rhi::Blend::Zero:           return D3D12_BLEND_ZERO;
      case rex::rhi::Blend::One:            return D3D12_BLEND_ONE;
      case rex::rhi::Blend::SrcColor:       return D3D12_BLEND_SRC_COLOR;
      case rex::rhi::Blend::InvSrcColor:    return D3D12_BLEND_INV_SRC_COLOR;
      case rex::rhi::Blend::SrcAlpha:       return D3D12_BLEND_SRC_ALPHA;
      case rex::rhi::Blend::InvSrcAlpha:    return D3D12_BLEND_INV_SRC_ALPHA;
      case rex::rhi::Blend::DestAlpha:      return D3D12_BLEND_DEST_ALPHA;
      case rex::rhi::Blend::InvDestAlpha:   return D3D12_BLEND_INV_DEST_ALPHA;
      case rex::rhi::Blend::DestColor:      return D3D12_BLEND_DEST_COLOR;
      case rex::rhi::Blend::InvDestColor:   return D3D12_BLEND_INV_DEST_COLOR;
      case rex::rhi::Blend::SrcAlphaSat:    return D3D12_BLEND_SRC_ALPHA_SAT;
      case rex::rhi::Blend::BlendFactor:    return D3D12_BLEND_BLEND_FACTOR;
      case rex::rhi::Blend::InvBlendFactor: return D3D12_BLEND_INV_BLEND_FACTOR;
      case rex::rhi::Blend::Src1Color:      return D3D12_BLEND_SRC1_COLOR;
      case rex::rhi::Blend::InvSrc1Color:   return D3D12_BLEND_INV_SRC1_COLOR;
      case rex::rhi::Blend::Src1Alpha:      return D3D12_BLEND_SRC1_ALPHA;
      case rex::rhi::Blend::InvSrc1Alpha:   return D3D12_BLEND_INV_SRC1_ALPHA;
      default:
        break;
      }
     
      return invalid_obj<D3D12_BLEND>();
    }
    D3D12_BLEND_OP to_dx12(rhi::BlendOp blendOp)
    {
      switch (blendOp)
      {
      case rex::rhi::BlendOp::Add:          return D3D12_BLEND_OP_ADD;
      case rex::rhi::BlendOp::Subtract:     return D3D12_BLEND_OP_SUBTRACT;
      case rex::rhi::BlendOp::RevSubtract:  return D3D12_BLEND_OP_REV_SUBTRACT;
      case rex::rhi::BlendOp::Min:          return D3D12_BLEND_OP_MIN;
      case rex::rhi::BlendOp::Max:          return D3D12_BLEND_OP_MAX;
      }

      return invalid_obj<D3D12_BLEND_OP>();
    }
    D3D12_LOGIC_OP to_dx12(rhi::LogicOp logicOp)
    {
      switch (logicOp)
      {
      case rex::rhi::LogicOp::Clear:          return D3D12_LOGIC_OP_CLEAR;
      case rex::rhi::LogicOp::Set:            return D3D12_LOGIC_OP_SET;
      case rex::rhi::LogicOp::Copy:           return D3D12_LOGIC_OP_COPY;
      case rex::rhi::LogicOp::CopyInverted:   return D3D12_LOGIC_OP_COPY_INVERTED;
      case rex::rhi::LogicOp::Noop:           return D3D12_LOGIC_OP_NOOP;
      case rex::rhi::LogicOp::Invert:            return D3D12_LOGIC_OP_INVERT;
      case rex::rhi::LogicOp::And:            return D3D12_LOGIC_OP_AND;
      case rex::rhi::LogicOp::Nand:           return D3D12_LOGIC_OP_NAND;
      case rex::rhi::LogicOp::Or:             return D3D12_LOGIC_OP_OR;
      case rex::rhi::LogicOp::Nor:            return D3D12_LOGIC_OP_NOR;
      case rex::rhi::LogicOp::Xor:            return D3D12_LOGIC_OP_XOR;
      case rex::rhi::LogicOp::Equiv:          return D3D12_LOGIC_OP_EQUIV;
      case rex::rhi::LogicOp::AndReverse:     return D3D12_LOGIC_OP_AND_REVERSE;
      case rex::rhi::LogicOp::AndInverted:    return D3D12_LOGIC_OP_AND_INVERTED;
      case rex::rhi::LogicOp::OrReverse:      return D3D12_LOGIC_OP_OR_REVERSE;
      case rex::rhi::LogicOp::OrInverted:     return D3D12_LOGIC_OP_OR_INVERTED;
      }

      return invalid_obj<D3D12_LOGIC_OP>();
    }
    D3D12_DEPTH_STENCIL_DESC to_dx12(const rhi::DepthStencilDesc& depthStencilState)
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

    D3D12_DEPTH_WRITE_MASK to_dx12(rhi::DepthWriteMask mask)
    {
      switch (mask)
      {
      case rex::rhi::DepthWriteMask::DepthWriteMaskZero: return D3D12_DEPTH_WRITE_MASK_ZERO;
      case rex::rhi::DepthWriteMask::DepthWriteMaskAll: return D3D12_DEPTH_WRITE_MASK_ALL;
      }

      return invalid_obj<D3D12_DEPTH_WRITE_MASK>();
    }

    D3D12_DEPTH_STENCILOP_DESC to_dx12(const rhi::DepthStencilOpDesc& depthStencilOp)
    {
      D3D12_DEPTH_STENCILOP_DESC desc{};

      desc.StencilDepthFailOp = to_dx12(depthStencilOp.stencil_depth_fail_op);
      desc.StencilPassOp = to_dx12(depthStencilOp.stencil_pass_op);
      desc.StencilFailOp = to_dx12(depthStencilOp.stencil_fail_op);
      desc.StencilFunc = to_dx12(depthStencilOp.stencil_func);

      return desc;
    }
    D3D12_STENCIL_OP to_dx12(rhi::StencilOp stencilOp)
    {
      switch (stencilOp)
      {
      case rex::rhi::StencilOp::Keep:     return D3D12_STENCIL_OP_KEEP;
      case rex::rhi::StencilOp::Zero:     return D3D12_STENCIL_OP_ZERO;
      case rex::rhi::StencilOp::Replace:  return D3D12_STENCIL_OP_REPLACE;
      case rex::rhi::StencilOp::IncrSat:  return D3D12_STENCIL_OP_INCR_SAT;
      case rex::rhi::StencilOp::DecrSat:  return D3D12_STENCIL_OP_DECR_SAT;
      case rex::rhi::StencilOp::Invert:   return D3D12_STENCIL_OP_INVERT;
      case rex::rhi::StencilOp::Incr:     return D3D12_STENCIL_OP_INCR;
      case rex::rhi::StencilOp::Decr:     return D3D12_STENCIL_OP_DECR;
      }

      return invalid_obj<D3D12_STENCIL_OP>();
    }

    D3D12_DESCRIPTOR_RANGE to_dx12(rhi::DescriptorRangeDesc range)
    {
      D3D12_DESCRIPTOR_RANGE range_desc{};
      range_desc.BaseShaderRegister = range.base_shader_register;
      range_desc.NumDescriptors = range.num_descriptors;
      range_desc.OffsetInDescriptorsFromTableStart = range.offset_in_descriptors_from_table_start;
      range_desc.RangeType = to_dx12(range.type);
      range_desc.RegisterSpace = range.register_space;

      return range_desc;
    }

    D3D12_DESCRIPTOR_RANGE_TYPE to_dx12(rhi::DescriptorRangeType type)
    {
      switch (type)
      {
      case rex::rhi::DescriptorRangeType::ConstantBufferView:   return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
      case rex::rhi::DescriptorRangeType::ShaderResourceView:   return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
      case rex::rhi::DescriptorRangeType::UnorderedAccessView:  return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
      case rex::rhi::DescriptorRangeType::Sampler:              return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
      }

      return invalid_obj<D3D12_DESCRIPTOR_RANGE_TYPE>();
    }

    D3D12_RESOURCE_STATES to_dx12(ResourceState state)
    {
      switch (state)
      {
      case rex::ResourceState::Common:                                  return D3D12_RESOURCE_STATE_COMMON                                   ;
      case rex::ResourceState::VertexAndConstantBuffer:                 return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER               ;
      case rex::ResourceState::IndexBuffer:                             return D3D12_RESOURCE_STATE_INDEX_BUFFER                             ;
      case rex::ResourceState::RenderTarget:                            return D3D12_RESOURCE_STATE_RENDER_TARGET                            ;
      case rex::ResourceState::UnorderedAccess:                         return D3D12_RESOURCE_STATE_UNORDERED_ACCESS                         ;
      case rex::ResourceState::DepthWrite:                              return D3D12_RESOURCE_STATE_DEPTH_WRITE                              ;
      case rex::ResourceState::DepthRead:                               return D3D12_RESOURCE_STATE_DEPTH_READ                               ;
      case rex::ResourceState::NonPixelShaderResource:                  return D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE                ;
      case rex::ResourceState::PixelShaderResource:                     return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE                    ;
      case rex::ResourceState::StreamOut:                               return D3D12_RESOURCE_STATE_STREAM_OUT                               ;
      case rex::ResourceState::IndirectArgument:                        return D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT                        ;
      case rex::ResourceState::CopyDest:                                return D3D12_RESOURCE_STATE_COPY_DEST                                ;
      case rex::ResourceState::CopySource:                              return D3D12_RESOURCE_STATE_COPY_SOURCE                              ;
      case rex::ResourceState::ResolveDest:                             return D3D12_RESOURCE_STATE_RESOLVE_DEST                             ;
      case rex::ResourceState::ResolveSource:                           return D3D12_RESOURCE_STATE_RESOLVE_SOURCE                           ;
      case rex::ResourceState::RaytracingAccelerationStructure:         return D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE        ;
      case rex::ResourceState::ShadingRateSource:                       return D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE                      ;
      case rex::ResourceState::GenericRead:                             return D3D12_RESOURCE_STATE_GENERIC_READ                             ;
      case rex::ResourceState::Present:                                 return D3D12_RESOURCE_STATE_PRESENT                                  ;
      case rex::ResourceState::Predication:                             return D3D12_RESOURCE_STATE_PREDICATION                              ;
      case rex::ResourceState::VideoDecodeRead:                         return D3D12_RESOURCE_STATE_VIDEO_DECODE_READ                        ;
      case rex::ResourceState::VideoDecodeWrite:                        return D3D12_RESOURCE_STATE_VIDEO_DECODE_WRITE                       ;
      case rex::ResourceState::VideoProcessRead:                        return D3D12_RESOURCE_STATE_VIDEO_PROCESS_READ                       ;
      case rex::ResourceState::VideoProcessWrite:                       return D3D12_RESOURCE_STATE_VIDEO_PROCESS_WRITE                      ;
      case rex::ResourceState::VideoEncodeRead:                         return D3D12_RESOURCE_STATE_VIDEO_ENCODE_READ                        ;
      case rex::ResourceState::VideoEncodeWrite:                        return D3D12_RESOURCE_STATE_VIDEO_ENCODE_WRITE                       ;
      default:
        break;
      }

      return invalid_obj<D3D12_RESOURCE_STATES>();
    }

  }
} // namespace rex