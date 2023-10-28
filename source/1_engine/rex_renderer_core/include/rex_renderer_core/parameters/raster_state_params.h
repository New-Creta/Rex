#pragma once

#include "rex_engine/types.h"

#include "rex_renderer_core/fill_mode.h"
#include "rex_renderer_core/cull_mode.h"

namespace rex
{
    namespace renderer
    {
        namespace parameters
        {
            struct RasterState
            {
                FillMode fill_mode = FillMode::SOLID;
                CullMode cull_mode = CullMode::BACK;
                s32 front_ccw = 0;
                s32 depth_bias = 0;
                f32 depth_bias_clamp = 0.0f;
                f32 sloped_scale_depth_bias = 0.0f;
                s32 depth_clip_enable = 1;
                s32 multisample = 0;
                s32 aa_lines = 0;
                s32 forced_sample_count = 0;
            };
        }
    }
}