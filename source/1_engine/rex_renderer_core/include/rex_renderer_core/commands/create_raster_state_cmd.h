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
        RenderCommandDesc command;

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
        CreateRasterState(CreateRasterStateCommandDesc&& desc)
            : RenderCommand(rsl::move(desc.command))
            , m_desc(rsl::move(desc))
        {
        }

        ~CreateRasterState() override = default;

        bool execute() override 
        {
          return backend::create_raster_state(cmd.raster_state_params, cmd.resource_slot);  
        }

      private:
        CreateRasterStateCommandDesc m_desc;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex