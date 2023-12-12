#pragma once

#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_renderer_core/cull_mode.h"
#include "rex_renderer_core/fill_mode.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct CreateRasterStateCommandDesc
      {
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