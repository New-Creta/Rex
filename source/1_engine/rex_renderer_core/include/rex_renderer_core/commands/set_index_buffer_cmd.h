#pragma once

#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_renderer_core/index_buffer_format.h"
#include "rex_renderer_core/resource_slot.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct SetIndexBufferCommandDesc
      {
        RenderCommandDesc command;

        ResourceSlot m_buffer_index;
        IndexBufferFormat m_format;
        s32 m_offset;
      };

      class SetIndexBuffer : public RenderCommand
      {
      public:
        SetIndexBuffer(SetIndexBufferCommandDesc&& desc)
            : RenderCommand(rsl::move(desc.command))
            , m_desc(rsl::move(desc))
        {
        }

        ~SetIndexBuffer() override = default;

        bool execute() override
        {
          return backend::set_index_buffer(cmd.set_index_buffer.buffer_index.slot_id(), cmd.set_index_buffer.format, cmd.set_index_buffer.offset);
        }

      private:
        SetIndexBufferCommandDesc m_desc;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex