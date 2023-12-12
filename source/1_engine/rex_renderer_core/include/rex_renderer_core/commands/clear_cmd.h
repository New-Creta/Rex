#pragma once

#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_renderer_core/resource_slot.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct ClearCommandDesc
      {
        RenderCommandDesc command;

        ResourceSlot clear_state;
        s32 array_index;
        s32 texture_index;
      };

      class Clear : public RenderCommand
      {
      public:
        Clear(ClearCommandDesc&& desc)
            : RenderCommand(rsl::move(desc.command))
            , m_desc(rsl::move(desc))
        {
        }

        ~Clear() override = default;

        bool execute() override
        {
          backend::clear(clear_state.slot_id());

          return true;
        }

      private:
        ClearCommandDesc m_desc;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex