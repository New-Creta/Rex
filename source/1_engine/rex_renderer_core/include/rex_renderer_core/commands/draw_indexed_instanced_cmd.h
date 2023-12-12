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
            : RenderCommand()
            , m_desc(rsl::move(desc))
        {
        }

        ~DrawIndexedInstanced() override = default;

        bool execute() override
        {
          backend::draw_indexed_instanced(m_desc.instance_count, m_desc.start_instance, m_desc.index_count, m_desc.start_index, m_desc.base_vertex);

          return true;
        }

      private:
        DrawIndexedInstancedCommandDesc m_desc;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex