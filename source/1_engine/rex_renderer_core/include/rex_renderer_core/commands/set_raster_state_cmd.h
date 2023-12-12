#pragma once

#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_renderer_core/resource_slot.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct SetRasterStateCommandDesc
      {
        RenderCommandDesc command;

        ResourceSlot raster_state;
      };

      class SetRasterState : public RenderCommand
      {
      public:
        SetRasterState(SetRasterStateCommandDesc&& desc)
            : RenderCommand(rsl::move(desc.command))
            , m_desc(rsl::move(desc))
        {
        }

        ~SetRasterState() override = default;

        bool execute() override 
        {
          return backend::set_raster_state(cmd.set_raster_state.raster_state.slot_id());
        }

      private:
        SetRasterStateCommandDesc m_desc;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex