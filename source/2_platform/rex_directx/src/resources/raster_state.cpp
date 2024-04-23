#include "rex_directx/resources/raster_state.h"

namespace rex
{
  namespace renderer
  {
    RasterState::RasterState(CreateRasterStateDesc&& desc)
        : m_desc(desc)
    {
    }

    RasterState::~RasterState() = default;

    FillMode RasterState::fill_mode() const 
    {
      return m_desc.fill_mode;
    }
    CullMode RasterState::cull_mode() const 
    {
      return m_desc.cull_mode;
    }
    s32 RasterState::front_ccw() const 
    {
      return m_desc.front_ccw;
    }
    s32 RasterState::depth_bias() const 
    {
      return m_desc.depth_bias;
    }
    f32 RasterState::depth_bias_clamp() const
    {
      return m_desc.depth_bias_clamp;
    }
    f32 RasterState::sloped_scale_depth_bias() const 
    {
      return m_desc.sloped_scale_depth_bias;
    }
    s32 RasterState::depth_clip_enable() const 
    {
      return m_desc.depth_clip_enable;
    }
    s32 RasterState::multisample() const 
    {
      return m_desc.multisample;
    }
    s32 RasterState::aa_lines() const 
    {
      return m_desc.aa_lines;
    }
    s32 RasterState::forced_sample_count() const 
    {
      return m_desc.forced_sample_count;
    }
    ConservativeRasterizationMode RasterState::conservative_raster() const 
    {
      return m_desc.conservative_raster;
    }
  } // namespace renderer
} // namespace rex