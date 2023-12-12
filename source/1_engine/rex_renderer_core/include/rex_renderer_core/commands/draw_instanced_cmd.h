#pragma once

#include "rex_renderer_core/commands/render_cmd.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct DrawInstanceCommandDesc
      {
        RenderCommandDesc command;

        s32 instance_count;
        s32 start_instance;
        s32 vertex_count;
        s32 start_vertex;
      };

      class DrawInstanced : public RenderCommand
      {
      public:
        DrawInstanced(DrawInstanceCommandDesc&& desc)
            : RenderCommand(rsl::move(desc.command))
            , m_desc(rsl::move(desc))
        {
        }

        ~DrawInstanced() override = default;

        bool execute() override 
        {
          REX_ASSERT_X(false, "Not implemented yet");
        }

      private:
        DrawInstanceCommandDesc m_desc;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex