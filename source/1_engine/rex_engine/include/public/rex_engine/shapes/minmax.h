#pragma once

#include "rex_engine/engine/types.h"
#include "rex_std/bonus/math.h"

namespace rex
{
  struct MinMax
  {
    rsl::pointi32 min;
    rsl::pointi32 max;

    s32 width() const
    {
      return max.x - min.x;
    }
    s32 height() const
    {
      return max.y - min.y;
    }
  };
}