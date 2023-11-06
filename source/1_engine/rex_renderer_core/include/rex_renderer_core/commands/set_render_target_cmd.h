#pragma once

#include "rex_engine/types.h"

namespace rex
{
    namespace renderer
    {
        namespace commands
        {
            static constexpr u32 s_max_color_targets = 8;

            struct SetRenderTarget
            {
                SetRenderTarget()
                    :num_color(0)
                    ,color()
                    ,depth(REX_INVALID_INDEX)
                    ,array_index(REX_INVALID_INDEX)
                {
                    memset(&color, (s32)REX_INVALID_INDEX, s_max_color_targets * sizeof(u32));
                }

                s32 num_color;
                s32 color[s_max_color_targets];
                s32 depth;
                s32 array_index;
            };
        }
    }
}