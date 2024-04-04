#pragma once

#include "rex_engine/engine/types.h"
#include "rex_renderer_core/rendering/fill_mode.h"
#include "rex_renderer_core/rendering/cull_mode.h"

namespace rex
{
  namespace rhi
  {
    struct RasterStateDesc
    {
      RasterStateDesc()
        : fill_mode(renderer::FillMode::SOLID)
        , cull_mode(renderer::CullMode::BACK)
        , front_ccw(0)
        , depth_bias(0)
        , depth_bias_clamp(0.0f)
        , sloped_scale_depth_bias(0.0f)
        , depth_clip_enable(1)
        , multisample(0)
        , aa_lines(0)
        , forced_sample_count(0)
      {
      }

      renderer::FillMode fill_mode;
      renderer::CullMode cull_mode;
      s32 front_ccw;
      s32 depth_bias;
      f32 depth_bias_clamp;
      f32 sloped_scale_depth_bias;
      s32 depth_clip_enable;
      s32 multisample;
      s32 aa_lines;
      s32 forced_sample_count;
    };
  }
}