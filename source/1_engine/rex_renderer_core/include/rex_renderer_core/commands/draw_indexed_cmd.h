#pragma once

#include "rex_renderer_core/commands/render_cmd.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct DrawIndexedCommandDesc
      {
        RenderCommandDesc command;

        s32 index_count;
        s32 start_index;
        s32 base_vertex;
      };

      class DrawIndexed : public RenderCommand
      {
      public:
        DrawIndexed(DrawIndexedCommandDesc&& desc)
            : RenderCommand(rsl::move(desc.command))
            , m_desc(rsl::move(desc))
        {
        }

        ~DrawIndexed() override = default;

        bool execute() override
        {
          backend::draw_indexed(cmd.draw_indexed.index_count, cmd.draw_indexed.start_index, cmd.draw_indexed.base_vertex);

          return true;
        }

      private:
        DrawIndexedCommandDesc m_desc;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex