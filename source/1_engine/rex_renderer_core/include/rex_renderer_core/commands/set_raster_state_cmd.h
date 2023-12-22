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
      };

      class SetRasterState : public RenderCommand
      {
      public:
        SetRasterState(SetRasterStateCommandDesc&& desc, const ResourceSlot& slot)
            : RenderCommand()
            , m_desc(rsl::move(desc))
            , m_resource_slot(slot)
        {
        }

        ~SetRasterState() override = default;

        bool execute() override 
        {
          return backend::set_raster_state(m_resource_slot);
        }

      private:
        SetRasterStateCommandDesc m_desc;
        ResourceSlot m_resource_slot;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex