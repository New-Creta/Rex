#pragma once

#include "rex_engine/engine/types.h"

namespace rex
{
  namespace renderer
  {
    struct ScissorRect
    {
      ScissorRect()
          : left(0)
          , top(0)
          , right(0)
          , bottom(0)
      {
      }

      ScissorRect(u64 l, u64 t, u64 r, u64 b)
          : left(l)
          , top(t)
          , right(r)
          , bottom(b)
      {
      }

      u64 left;
      u64 top;
      u64 right;
      u64 bottom;
    };
  } // namespace renderer
} // namespace rex