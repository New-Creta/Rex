#pragma once

#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_renderer_core/resource_slot.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct SetInputLayoutCommandDesc
      {
        RenderCommandDesc command;

        ResourceSlot input_layout;
      };

      class SetInputLayout : public RenderCommand
      {
      public:
        SetInputLayout(SetInputLayoutCommandDesc&& desc)
            : RenderCommand(rsl::move(desc.command))
            , m_desc(rsl::move(desc))
        {
        }

        ~SetInputLayout() override = default;

        bool execute() override 
        {
          return backend::set_input_layout(cmd.set_input_layout.input_layout.slot_id());
        }

      private:
        SetInputLayoutCommandDesc m_desc;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex