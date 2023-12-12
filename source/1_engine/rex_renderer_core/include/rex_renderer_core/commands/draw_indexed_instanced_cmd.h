#pragma once

#include "rex_renderer_core/commands/render_cmd.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct DrawIndexedInstancedCommandDesc
      {
        RenderCommandDesc command;

        s32 instance_count;
        s32 start_instance;
        s32 index_count;
        s32 start_index;
        s32 base_vertex;
      };

      class DrawIndexedInstanced : public RenderCommand
      {
      public:
        DrawIndexedInstanced(DrawIndexedInstancedCommandDesc&& desc)
            : RenderCommand(rsl::move(desc.command))
            , m_desc(rsl::move(desc))
        {
        }

        ~DrawIndexedInstanced() override = default;

        bool execute() override
        {
          backend::draw_indexed_instanced(cmd.draw_indexed_instanced.instance_count, cmd.draw_indexed_instanced.start_instance, cmd.draw_indexed_instanced.index_count, cmd.draw_indexed_instanced.start_index, cmd.draw_indexed_instanced.base_vertex);

          return true;
        }

      private:
        DrawIndexedInstancedCommandDesc m_desc;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex