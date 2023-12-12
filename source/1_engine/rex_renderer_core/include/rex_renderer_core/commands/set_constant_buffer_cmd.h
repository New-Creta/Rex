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
        s32 location;
      };
      class SetConstantBuffer : public RenderCommand
      {
      public:
        SetConstantBuffer(SetConstantBufferCommandDesc&& desc, ResourceSlot slot)
            : RenderCommand()
            , m_desc(rsl::move(desc))
            , m_resource_slot(slot)
        {
        }

        ~SetConstantBuffer() override = default;

        bool execute() override 
        {
          return backend::set_constant_buffer(m_resource_slot, m_desc.location);
        }

      private:
        SetConstantBufferCommandDesc m_desc;
        ResourceSlot m_resource_slot;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex