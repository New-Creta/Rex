#pragma once

#include "rex_engine/engine/types.h"
#include "rex_renderer_core/cull_mode.h"
#include "rex_renderer_core/fill_mode.h"

namespace rex
{
  namespace renderer
  {
    enum class ConservativeRasterizationMode
    {
      MODE_OFF = 0,
      MODE_ON  = 1
    };

    struct CreateRasterStateDesc
    {
      CreateRasterStateDesc()
          : fill_mode(FillMode::SOLID)
          , cull_mode(CullMode::BACK)
          , front_ccw(0)
          , depth_bias(0)
          , depth_bias_clamp(0.0f)
          , sloped_scale_depth_bias(0.0f)
          , depth_clip_enable(1)
          , multisample(0)
          , aa_lines(0)
          , forced_sample_count(0)
          , conservative_raster(ConservativeRasterizationMode::MODE_OFF)
      {
      }

      FillMode fill_mode;
      CullMode cull_mode;
      s32 front_ccw;
      s32 depth_bias;
      f32 depth_bias_clamp;
      f32 sloped_scale_depth_bias;
      s32 depth_clip_enable;
      s32 multisample;
      s32 aa_lines;
      s32 forced_sample_count;
      ConservativeRasterizationMode conservative_raster;
    };
  } // namespace renderer
} // namespace rex