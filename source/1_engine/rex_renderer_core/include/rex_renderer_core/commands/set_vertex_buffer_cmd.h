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
        s32 buffer_index;
        s32 start_slot;
        rsl::vector<ResourceSlot> vertex_buffer_targets;
        rsl::vector<s32> strides;
        rsl::vector<s32> offsets;
      };

      class SetVertexBuffer : public RenderCommand
      {
      public:
        SetVertexBuffer(SetVertexBufferCommandDesc&& desc)
            : RenderCommand()
            , m_desc(rsl::move(desc))
        {
        }

        ~SetVertexBuffer() override = default;

        bool execute() override
        {
          return backend::set_vertex_buffers(m_desc.vertex_buffer_targets.data(), m_desc.vertex_buffer_targets.size(), m_desc.start_slot, m_desc.strides.data(), m_desc.offsets.data());
        }

      private:
        SetVertexBufferCommandDesc m_desc;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex