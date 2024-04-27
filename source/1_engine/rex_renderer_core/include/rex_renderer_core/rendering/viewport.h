#pragma once

#include "rex_engine/engine/types.h"

namespace rex
{
  // Graphics api abstraction of the viewport
  struct Viewport
  {
    Viewport()
        : top_left_x(0.0f)
        , top_left_y(0.0f)
        , width(0.0f)
        , height(0.0f)
        , min_depth(0.0f)
        , max_depth(0.0f)
    {
    }

    Viewport(f32 topLeftX, f32 topLeftY, f32 vpWith, f32 vpHeight, f32 minDepth, f32 maxDepth)
        : top_left_x(topLeftX)
        , top_left_y(topLeftY)
        , width(vpWith)
        , height(vpHeight)
        , min_depth(minDepth)
        , max_depth(maxDepth)
    {
    }

    f32 top_left_x;
    f32 top_left_y;

    f32 width;
    f32 height;

    f32 min_depth;
    f32 max_depth;
  };
} // namespace rex