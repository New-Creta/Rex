#pragma once

#include "rex_engine/engine/types.h"

namespace rex
{
  namespace renderer
  {
    struct DepthInfo
    {
      f32 near_plane = 1.0f;
      f32 far_plane  = 1000.0f;
    };
  } // namespace renderer
} // namespace rex