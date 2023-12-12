#pragma once

#include "rex_renderer_core/commands/render_cmd.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct DrawCommandDesc
      {
        s32 vertex_count;
        s32 start_vertex;
      };

      class Draw : public RenderCommand
      {
      public:
        Draw(DrawCommandDesc&& desc)
            : RenderCommand()
            , m_desc(rsl::move(desc))
        {
        }

        ~Draw() override = default;

        bool execute() override 
        {
          backend::draw(m_desc.vertex_count, m_desc.start_vertex);

          return true;
        }

      private:
        DrawCommandDesc m_desc;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex