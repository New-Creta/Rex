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
        DrawCommandDesc()
            :vertex_count(0)
            ,start_vertex(0)
        {}

        DrawCommandDesc(s32 vertexCount, s32 startVertex)
            :vertex_count(vertexCount)
            , start_vertex(startVertex)
        {}

        s32 vertex_count;
        s32 start_vertex;
      };

      class Draw : public RenderCommand
      {
      public:
        explicit Draw(DrawCommandDesc&& desc)
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