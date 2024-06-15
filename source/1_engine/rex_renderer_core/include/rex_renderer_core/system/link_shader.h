#pragma once

#include "rex_renderer_core/resource_management/resource_slot.h"
#include "rex_std/memory.h"
#include "rex_std/string_view.h"

#include "rex_renderer_core/gfx/shader_visibility.h"
#include "rex_renderer_core/gfx/sampler_filtering.h"
#include "rex_renderer_core/gfx/comparison_func.h"
#include "rex_renderer_core/gfx/border_color.h"
#include "rex_renderer_core/gfx/texture_address_mode.h"

namespace rex
{
  namespace gfx
  {
    // The type of view for a resource tied to a shader
    enum class ShaderViewType
    {
      ConstantBufferView,
      ShaderResourceView,
      UnorderedAccessView
    };

    // The type of views that are defined in the range
    enum class DescriptorRangeType
    {
      ConstantBufferView,
      ShaderResourceView,
      UnorderedAccessView,
      Sampler
    };

    // Describes a shader constant (eg. a float array)
    struct ShaderConstantDesc
    {
      rsl::string_view name{}; // The name of the shader constant
      s32 reg; // The register of the shader constant
      s32 reg_space; // The register space of the shader constant, this is often 0
      s32 num_32bits; // The number of constants that occupy a sigle shader slot
      ShaderVisibility visibility; // the shader or shaders this view is visible in.
    };

    // Describes a shader view parameter (eg. constant buffer view)
    struct ShaderViewDesc
    {
      rsl::string_view name{};  // The name of the shader view
      ShaderViewType type{}; // The type of shader view
      s32 reg{}; // the register of the shader view
      s32 reg_space{}; // the register space of the shader view, this is often 0
      ShaderVisibility visibility; // the shader or shaders this view is visible in.
    };

    // Describe a descriptor/view range (2 constant buffer views)
    struct DescriptorRangeDesc
    {
      DescriptorRangeType type{}; // The type of descriptor/view we have a range over
      s32 num_descriptors{}; // the number of descriptors/views in the range
      s32 base_shader_register{}; // The register of the first descriptor/view in the range.
      s32 offset_in_descriptors_from_table_start{}; // The index in the descriptor table of the first descriptor the range covers
      s32 register_space{}; // the register space for every descriptor/view in the range, this is often 0
    };

    // Describe a descriptor table, which is a list of descriptor ranges
    struct DescriptorTableDesc
    {
      rsl::unique_array<DescriptorRangeDesc> ranges; // All the ranges of the descriptor table
      ShaderVisibility visibility; // The shader types where the ranges are visible
    };

    // Describe a static sampler for a shader
    struct ShaderSamplerDesc
    {
      SamplerFiltering filtering = SamplerFiltering::Anisotropic; // The sampler's filtering method
      TextureAddressMode address_mode_u = TextureAddressMode::Wrap; // The sampler's address mode for the u component
      TextureAddressMode address_mode_v = TextureAddressMode::Wrap; // The sampler's address mode for the v component
      TextureAddressMode address_mode_w = TextureAddressMode::Wrap; // The sampler's address mode for the w component
      f32 mip_lod_bias = 0.0f; // Offset from calculated mip level. meaning if the gpu thinks it should sample at lvl 3 but this var is set to 2, the texture will be sampled at lvl 5
      s32 max_anisotropy = 0; // clamping value if using anisotrpoic filtering. values are valid between 1 and 16
      ComparisonFunc comparison_func = ComparisonFunc::Always; // A function that compares sampled data against existing sampled data. 
      BorderColor border_color = BorderColor::TransparentBlack; // The border color to use if any of the address mode is using TextureAddressMode::Border
      f32 min_lod = 0.0f; // lower end of the mip level range to use
      f32 max_lod = 0.0f; // higher end of the mip level range to use
      s32 shader_register = 0; // the shader register of the sampler
      s32 register_space = 0; // the shader register space of the sampler
      ShaderVisibility shader_visibility = ShaderVisibility::All; // the shaders this sampler is visible in
    };

    // Describe the root signature for all the shaders
    // This basically says what resources are needed for the graphics pipeline
    // It only specifies the type, not the actual data
    // for example:
    //
    // -- VERTEX SHADER --
    //   
    // cbuffer my_vertex_buffer : register(b0)
    // {
    //    .. some data here ..
    // };
    //
    // .. other vertex shader code ..
    //
    // 
    // 
    // -- PIXEL SHADER -- 
    //
    // SamplerState sampler0 : register(s0);
    // Texture2D texture0 : register(t0);
    //
    // .. other pixel shader code ..
    //
    //
    // The above would result in a root signature defining
    // a constant buffer, a sampler and a texture 2d as its parameters.
    // 
    // A root signature holds all the parameters for every shader type bound to a pipeline.
    struct RootSignatureDesc
    {
      rsl::unique_array<ShaderConstantDesc> constants; // all the constants of the shaders
      rsl::unique_array<ShaderViewDesc> views; // All the views of the shaders
      rsl::unique_array<DescriptorTableDesc> desc_tables; // all the descriptor tables describing the various views and samplers of the shaders
      rsl::unique_array<ShaderSamplerDesc> samplers; // All the samplers of the shaders
    };
  } // namespace gfx
} // namespace rex