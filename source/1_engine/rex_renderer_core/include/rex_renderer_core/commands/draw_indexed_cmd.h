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
        s32 index_count;
        s32 start_index;
        s32 base_vertex;
      };

      class DrawIndexed : public RenderCommand
      {
      public:
        DrawIndexed(DrawIndexedCommandDesc&& desc)
            : RenderCommand()
            , m_desc(rsl::move(desc))
        {
        }

        ~DrawIndexed() override = default;

        bool execute() override
        {
          backend::draw_indexed(m_desc.index_count, m_desc.start_index, m_desc.base_vertex);

          return true;
        }

      private:
        DrawIndexedCommandDesc m_desc;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex