#pragma once

#include "rex_engine/engine/types.h"

namespace rex
{
  namespace gfx
  {
    // Depth info is just settings of the near plance and the far plane
    struct DepthInfo
    {
      f32 near_plane = 1.0f;
      f32 far_plane  = 1000.0f;
    };
  } // namespace gfx
} // namespace rex