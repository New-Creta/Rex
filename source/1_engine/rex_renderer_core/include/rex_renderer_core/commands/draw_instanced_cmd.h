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
        s32 instance_count;
        s32 start_instance;
        s32 vertex_count;
        s32 start_vertex;
      };

      class DrawInstanced : public RenderCommand
      {
      public:
        DrawInstanced(DrawInstanceCommandDesc&& desc)
            : RenderCommand()
            , m_desc(rsl::move(desc))
        {
        }

        ~DrawInstanced() override = default;

        bool execute() override 
        {
            backend::draw_instanced(m_desc.vertex_count, m_desc.instance_count, m_desc.start_vertex, m_desc.start_instance);

            return true;
        }

      private:
        DrawInstanceCommandDesc m_desc;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex