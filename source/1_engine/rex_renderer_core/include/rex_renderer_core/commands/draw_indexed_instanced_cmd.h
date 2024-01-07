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
          DrawIndexedInstancedCommandDesc()
              : instance_count(0)
              , start_instance(0)
              , index_count(0)
              , start_index(0)
              , base_vertex(0)
          {}

          DrawIndexedInstancedCommandDesc(s32 instanceCount, s32 startInstance, s32 indexCount, s32 startIndex, s32 baseVertex)
              : instance_count(instanceCount)
              , start_instance(startInstance)
              , index_count(indexCount)
              , start_index(startIndex)
              , base_vertex(baseVertex)
          {}

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