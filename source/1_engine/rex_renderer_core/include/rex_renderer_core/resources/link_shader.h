#pragma once

#include "rex_renderer_core/resource_management/resource_slot.h"
#include "rex_std/memory.h"
#include "rex_std/string_view.h"

#include "rex_renderer_core/rendering/shader_visibility.h"
#include "rex_renderer_core/rendering/sampler_filtering.h"
#include "rex_renderer_core/rendering/comparison_func.h"
#include "rex_renderer_core/rendering/border_color.h"
#include "rex_renderer_core/rendering/texture_address_mode.h"

namespace rex
{
  namespace rhi
  {
    enum class ShaderParameterType
    {
      Constant = 0,
      CBuffer,
      ShaderResourceView,
      UnorderedAccessView
    };

    enum class DescriptorRangeType
    {
      ConstantBufferView,
      ShaderResourceView,
      UnorderedAccessView,
      Sampler
    };

    struct ShaderParameterLayoutDescription
    {
      ShaderParameterType type{};

      rsl::string_view name{};
      s32 reg{};

      renderer::ShaderVisibility visibility;
      s32 space{};
      s32 num_32bits_for_constant;
    };

    struct DescriptorRange
    {
      DescriptorRangeType type{};
      s32 num_descriptors{};
      s32 base_shader_register{};
      s32 offset_in_descriptors_from_table_start{};
      s32 register_space{};
    };

    struct DescriptorTableDescription
    {
      rsl::unique_array<DescriptorRange> ranges;
      renderer::ShaderVisibility visibility;
    };

    struct ShaderSamplerDescription
    {
      renderer::SamplerFiltering filtering = renderer::SamplerFiltering::Anisotropic;
      renderer::TextureAddressMode address_mode_u = renderer::TextureAddressMode::Wrap;
      renderer::TextureAddressMode address_mode_v = renderer::TextureAddressMode::Wrap;
      renderer::TextureAddressMode address_mode_w = renderer::TextureAddressMode::Wrap;
      f32 mip_lod_bias = 0.0f;
      s32 max_anisotropy = 0;
      renderer::ComparisonFunc comparison_func = renderer::ComparisonFunc::Always;
      renderer::BorderColor border_color = renderer::BorderColor::TransparentBlack;
      f32 min_lod = 0.0f;
      f32 max_lod = 0.0f;
      s32 shader_register = 0;
      s32 register_space = 0;
      renderer::ShaderVisibility shader_visibility = renderer::ShaderVisibility::All;
    };

    struct LinkShaderDesc
    {
      rhi::ResourceSlot vertex_shader;
      rhi::ResourceSlot pixel_shader;
      rsl::unique_array<ShaderParameterLayoutDescription> constants;
      rsl::unique_array<DescriptorTableDescription> desc_tables;
      rsl::unique_array<rhi::ShaderSamplerDescription> samplers;
    };
  } // namespace rhi
} // namespace rex