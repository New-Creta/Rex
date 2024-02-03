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
        ScissorRect scissor_rect;
      };

      class SetScissorRect : public RenderCommand
      {
      public:
        explicit SetScissorRect(SetScissorRectCommandDesc&& desc)
            : RenderCommand()
            , m_desc(rsl::move(desc))
        {
        }

        ~SetScissorRect() override = default;

        bool execute() override 
        {
          return backend::set_scissor_rect(m_desc.scissor_rect);
        }

      private:
        SetScissorRectCommandDesc m_desc;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex