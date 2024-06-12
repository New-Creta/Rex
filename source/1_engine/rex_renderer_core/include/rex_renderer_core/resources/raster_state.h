#pragma once

#include "rex_engine/engine/types.h"
#include "rex_renderer_core/rhi/fill_mode.h"
#include "rex_renderer_core/rhi/cull_mode.h"

namespace rex
{
  namespace rhi
  {
    // A descriptor describing the raster state to be constructed
    struct RasterStateDesc
    {
      RasterStateDesc()
        : fill_mode(renderer::FillMode::Solid)
        , cull_mode(renderer::CullMode::Back)
        , depth_bias(0)
        , depth_bias_clamp(0.0f)
        , sloped_scale_depth_bias(0.0f)
        , forced_sample_count(0)
        , multisample_enable(false)
        , aa_lines_enable(false)
        , depth_clip_enable(true)
        , front_ccw(false)
      {
      }

      renderer::FillMode fill_mode; // Describe the fill mode of a primitive
      renderer::CullMode cull_mode; // Describe the cull mode of a primitive
      s32 depth_bias;               // Depth value added to a given pixel
      f32 depth_bias_clamp;         // Maximum depth bias of a pixel
      f32 sloped_scale_depth_bias;  // scalar on a given pixel's slope
      s32 forced_sample_count;      // The sample count that is forced while UAV rnedering or rasterizing (valid values are: 0, 1, 4, 8) 
      bool multisample_enable;      // Specifies whether to use quadrilateral or alpha line anti-aliasing algorithm on multi sample antialiasing
      bool aa_lines_enable;         // Specifies whether to enable line antialiasing
      bool depth_clip_enable;       // specifies whether to enable clipping based on distance
      bool front_ccw;               // Describe if a triangle is front or back-facing
    };

    // A raster state
    class RasterState
    {
    public:
      RasterState(const RasterStateDesc& desc)
        : m_desc(desc)
      {}

      const RasterStateDesc& get() const
      {
        return m_desc;
      }

    private:
      RasterStateDesc m_desc;
    };
  }
}