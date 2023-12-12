#pragma once

#include "rex_renderer_core/commands/render_cmd.h"

#include <rex_std/vector.h>

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      enum class ConstantType
      {
        SAMPLER_2D = 0,
        SAMPLER_3D,
        SAMPLER_CUBE,
        SAMPLER_2DMS,
        SAMPLER_2D_ARRAY,
        SAMPLER_2D_DEPTH,
        SAMPLER_2D_DEPTH_ARRAY,
        CBUFFER,
        CONSTANT
      };

      struct ConstantLayoutDescription
      {
        ConstantType type;

        char8* name;
        s32 location;
      };

      using ConstantLayoutDescriptions = rsl::vector<ConstantLayoutDescription>;
    } // namespace commands
  }   // namespace renderer
} // namespace rex