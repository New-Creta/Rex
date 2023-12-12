#pragma once

#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_renderer_core/resource_slot.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct SetConstantBufferCommandDesc
      {
        RenderCommandDesc command;

        ResourceSlot target;
        s32 location;
      };
      class SetConstantBuffer : public RenderCommand
      {
      public:
        SetConstantBuffer(SetConstantBufferCommandDesc&& desc)
            : RenderCommand(rsl::move(desc.command))
            , m_desc(rsl::move(desc))
        {
        }

        ~SetConstantBuffer() override = default;

        bool execute() override 
        {
          return backend::set_constant_buffer(cmd.set_constant_buffer.target.slot_id(), cmd.set_constant_buffer.location);
        }

      private:
        SetConstantBufferCommandDesc m_desc;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex