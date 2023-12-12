#pragma once

#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_renderer_core/viewport.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct SetViewportCommandDesc
      {
        RenderCommandDesc command;

        Viewport viewport;
      };

      class SetViewport : public RenderCommand
      {
      public:
        SetViewport(SetViewportCommandDesc&& desc)
            : RenderCommand(rsl::move(desc.command))
            , m_desc(rsl::move(desc))
        {
        }

        ~SetViewport() override = default;

        bool execute() override 
        {
          return backend::set_viewport(cmd.viewport);
        }

      private:
        SetViewportCommandDesc m_desc;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex