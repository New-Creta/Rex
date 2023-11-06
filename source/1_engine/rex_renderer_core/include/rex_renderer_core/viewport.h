#pragma once

#include "rex_engine/types.h"

namespace rex
{
    struct Viewport
    {
        f32 top_left_x;
        f32 top_left_y;

        f32 width;
        f32 height;

        f32 min_depth;
        f32 max_depth;
    };
} // namespace rex