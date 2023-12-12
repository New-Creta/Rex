#pragma once

#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_renderer_core/resource_slot.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct SetVertexBufferCommandDesc
      {
        RenderCommandDesc command;

        s32 buffer_index;
        s32 start_slot;
        ResourceSlot* buffer_indices;
        s32 num_buffers;
        s32* strides;
        s32* offsets;
      };

      class SetVertexBuffer : public RenderCommand
      {
      public:
        SetVertexBuffer(SetVertexBufferCommandDesc&& desc)
            : RenderCommand(rsl::move(desc.command))
            , m_desc(rsl::move(desc))
        {
        }

        ~SetVertexBuffer() override = default;

        bool execute() override
        {
          rsl::vector<s32> buffer_indices((rsl::Capacity)cmd.set_vertex_buffer.num_buffers);
          for(s32 i = 0; i < cmd.set_vertex_buffer.num_buffers; ++i)
            buffer_indices.push_back(cmd.set_vertex_buffer.buffer_indices[i].slot_id());

          result = backend::set_vertex_buffers(buffer_indices.data(), cmd.set_vertex_buffer.num_buffers, cmd.set_vertex_buffer.start_slot, cmd.set_vertex_buffer.strides, cmd.set_vertex_buffer.offsets);
          memory_free(cmd.set_vertex_buffer.buffer_indices);
          memory_free(cmd.set_vertex_buffer.strides);
          memory_free(cmd.set_vertex_buffer.offsets);

          return result;
        }

      private:
        SetVertexBufferCommandDesc m_desc;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex