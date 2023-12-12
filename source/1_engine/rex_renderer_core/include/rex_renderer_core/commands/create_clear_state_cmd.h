#pragma once

#include "rex_engine/state_controller.h"
#include "rex_renderer_core/clear_bits.h"
#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_std_extra/math/color.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct CreateClearStateCommandDesc
      {
        rsl::Color4f rgba;
        f32 depth;
        u8 stencil;
        StateController<ClearBits> flags;
      };

      class CreateClearState : public RenderCommand
      {
      public:
        CreateClearState(CreateClearStateCommandDesc&& desc, const ResourceSlot& slot)
            : RenderCommand()
            , m_desc(rsl::move(desc))
            , m_resource_slot(slot)
        {
        }

        ~CreateClearState() override = default;

        bool execute() override 
        {
          return backend::create_clear_state(m_desc, m_resource_slot);
        }

      private:
        CreateClearStateCommandDesc m_desc;
        ResourceSlot m_resource_slot;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex