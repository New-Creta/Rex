#pragma once

#include "rex_renderer_core/resource_management/resource_slot.h"
#include "rex_std/vector.h"

namespace rex
{
  namespace rhi
  {
    enum class ConstantType
    {
      Sampler2D = 0,
      Sampler3D,
      SamplerCube,
      Sampler2Dms,
      Sampler2DArray,
      Sampler2DDepth,
      Sampler2DDepthArray,
      CBuffer,
      Constant
    };

    struct ConstantLayoutDescription
    {
      ConstantType type{};

      rsl::string_view name{};
      s32 location{};
    };

    using ConstantLayoutDescriptions = rsl::vector<ConstantLayoutDescription>;

    struct LinkShaderDesc
    {
      rhi::ResourceSlot vertex_shader;
      rhi::ResourceSlot pixel_shader;
      rsl::unique_array<ConstantLayoutDescription> constants;
    };
  } // namespace rhi
} // namespace rex