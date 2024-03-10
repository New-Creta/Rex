#pragma once

#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_renderer_core/rendering/index_buffer_format.h"
#include "rex_renderer_core/resource_management/resource_slot.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct SetIndexBufferCommandDesc
      {
        SetIndexBufferCommandDesc()
            : format(IndexBufferFormat::None)
            , offset(0)
        {
        }

        SetIndexBufferCommandDesc(IndexBufferFormat indexBufferFormat, s32 indexBufferOffset)
            : format(indexBufferFormat)
            , offset(indexBufferOffset)
        {
        }

        IndexBufferFormat format;
        s32 offset;
      };

      class SetIndexBuffer : public RenderCommand
      {
      public:
        SetIndexBuffer(SetIndexBufferCommandDesc&& desc, const ResourceSlot& slot)
            : RenderCommand()
            , m_desc(rsl::move(desc))
            , m_resource_slot(slot)
        {
        }

        ~SetIndexBuffer() override = default;

        bool execute() override
        {
          return backend::set_index_buffer(m_resource_slot, m_desc.format, m_desc.offset);
        }

      private:
        SetIndexBufferCommandDesc m_desc;
        ResourceSlot m_resource_slot;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex