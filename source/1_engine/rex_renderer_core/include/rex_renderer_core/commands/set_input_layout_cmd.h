#pragma once

#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_renderer_resources/resource_slot.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct SetInputLayoutCommandDesc
      {
      };

      class SetInputLayout : public RenderCommand
      {
      public:
        SetInputLayout(SetInputLayoutCommandDesc&& desc, const ResourceSlot& slot)
            : RenderCommand()
            , m_desc(rsl::move(desc))
            , m_resource_slot(slot)
        {
        }

        ~SetInputLayout() override = default;

        bool execute() override
        {
          return backend::set_input_layout(m_resource_slot);
        }

      private:
        SetInputLayoutCommandDesc m_desc;
        ResourceSlot m_resource_slot;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex