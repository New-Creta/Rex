#pragma once

#include "rex_renderer_core/commands/render_cmd.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct EndDrawCommandDesc
      {
      };

      class EndDraw : public RenderCommand
      {
      public:
        EndDraw(EndDrawCommandDesc&& desc)
            : RenderCommand()
            , m_desc(rsl::move(desc))
        {
        }

        ~EndDraw() override = default;

        bool execute() override 
        {
          backend::end_draw();
        }

      private:
        EndDrawCommandDesc m_desc;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex