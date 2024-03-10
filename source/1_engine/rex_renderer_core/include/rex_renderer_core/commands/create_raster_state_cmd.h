#pragma once

#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_renderer_core/rendering/cull_mode.h"
#include "rex_renderer_core/rendering/fill_mode.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct CreateRasterStateCommandDesc
      {
        CreateRasterStateCommandDesc()
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
      };

      class CreateRasterState : public RenderCommand
      {
      public:
        CreateRasterState(CreateRasterStateCommandDesc&& desc, const ResourceSlot& slot)
            : RenderCommand()
            , m_desc(rsl::move(desc))
            , m_resource_slot(slot)
        {
        }

        ~CreateRasterState() override = default;

        bool execute() override
        {
          return backend::create_raster_state(m_desc, m_resource_slot);
        }

      private:
        CreateRasterStateCommandDesc m_desc;
        ResourceSlot m_resource_slot;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex