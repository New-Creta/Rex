#pragma once

#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_renderer_core/scissor_rect.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct SetScissorRectCommandDesc
      {
        RenderCommandDesc command;

        ScissorRect scissor_rect;
      };

      class SetScissorRect : public RenderCommand
      {
      public:
        SetScissorRect(SetScissorRectCommandDesc&& desc)
            : RenderCommand(rsl::move(desc.command))
            , m_desc(rsl::move(desc))
        {
        }

        ~SetScissorRect() override = default;

        bool execute() override 
        {
          return backend::set_scissor_rect(cmd.scissor_rect);
        }

      private:
        SetScissorRectCommandDesc m_desc;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex