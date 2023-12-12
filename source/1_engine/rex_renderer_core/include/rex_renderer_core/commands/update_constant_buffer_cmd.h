#pragma once

#include "rex_renderer_core/commands/render_cmd.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct UpdateConstantBufferCommandDesc
      {
        RenderCommandDesc command;

        void* data;
        s32 data_size;
        s32 element_index;
      };

      class UpdateConstantBuffer : public RenderCommand
      {
      public:
        UpdateConstantBuffer(UpdateConstantBufferCommandDesc&& desc)
            : RenderCommand(rsl::move(desc.command))
            , m_desc(rsl::move(desc))
        {
        }

        ~UpdateConstantBuffer() override = default;

        bool execute() override 
        {
          backend::update_constant_buffer(cmd.update_constant_buffer_params, cmd.resource_slot);

          return true;
        }

      private:
        UpdateConstantBufferCommandDesc m_desc;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex