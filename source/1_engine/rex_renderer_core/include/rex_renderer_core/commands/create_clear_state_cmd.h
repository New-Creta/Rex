#pragma once

#include "rex_engine/engine/state_controller.h"
#include "rex_renderer_core/clear_bits.h"
#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_std/bonus/math/color.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct CreateClearStateCommandDesc
      {
        CreateClearStateCommandDesc()
            : rgba(1.0f, 1.0f, 1.0f, 1.0f)
            , depth(1.0f)
            , stencil(0x00)
            , flags()
        {
          flags.add_state(renderer::ClearBits::ClearColorBuffer);
          flags.add_state(renderer::ClearBits::ClearDepthBuffer);
          flags.add_state(renderer::ClearBits::ClearStencilBuffer);
        }

        rsl::Color4f rgba;
        f32 depth;
        u8 stencil;
        StateController<ClearBits> flags {};
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