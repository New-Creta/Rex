#pragma once

#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_renderer_core/rendering/viewport.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct SetViewportCommandDesc
      {
        Viewport viewport;
      };

      class SetViewport : public RenderCommand
      {
      public:
        explicit SetViewport(SetViewportCommandDesc&& desc)
            : RenderCommand()
            , m_desc(rsl::move(desc))
        {
        }

        ~SetViewport() override = default;

        bool execute() override
        {
          return backend::set_viewport(m_desc.viewport);
        }

      private:
        SetViewportCommandDesc m_desc;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex