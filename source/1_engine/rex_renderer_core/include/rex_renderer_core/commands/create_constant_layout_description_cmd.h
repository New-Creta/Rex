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
        ConstantType type;

        rsl::string_view name;
        s32 location;
      };

      using ConstantLayoutDescriptions = rsl::vector<ConstantLayoutDescription>;
    } // namespace commands
  }   // namespace renderer
} // namespace rex