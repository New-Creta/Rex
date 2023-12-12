#pragma once

#include "rex_renderer_core/commands/render_cmd.h"

#include "rex_engine/memory/blob.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct UpdateConstantBufferCommandDesc
      {
        memory::Blob buffer_data;
        s32 element_index;
      };

      class UpdateConstantBuffer : public RenderCommand
      {
      public:
        UpdateConstantBuffer(UpdateConstantBufferCommandDesc&& desc, ResourceSlot slot)
            : RenderCommand()
            , m_desc(rsl::move(desc))
            , m_resource_slot(slot)
        {
        }

        ~UpdateConstantBuffer() override = default;

        bool execute() override 
        {
          backend::update_constant_buffer(m_desc, m_resource_slot);

          return true;
        }

      private:
        UpdateConstantBufferCommandDesc m_desc;
        ResourceSlot m_resource_slot;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex