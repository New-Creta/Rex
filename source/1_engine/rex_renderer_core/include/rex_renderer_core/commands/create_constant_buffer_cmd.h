#pragma once

#include "rex_engine/defines.h"
#include "rex_engine/state_controller.h"
#include "rex_renderer_core/commands/render_cmd.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct CreateConstantBufferCommandDesc
      {
        s32 count;
        s32 buffer_size;
        s32 array_index;
      };

      class CreateConstantBuffer : public RenderCommand
      {
      public:
        CreateConstantBuffer(CreateConstantBufferCommandDesc&& desc, ResourceSlot slot)
            : RenderCommand()
            , m_desc(rsl::move(desc))
            , m_resource_slot(slot)
        {
        }

        ~CreateConstantBuffer() override = default;

        bool execute() override 
        {
          return backend::create_constant_buffer(m_desc, m_resource_slot);
        }

      private:
        CreateConstantBufferCommandDesc m_desc;
        ResourceSlot m_resource_slot;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex