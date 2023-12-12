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
        RenderCommandDesc command;

        s32 count;
        s32 buffer_size;
        s32 array_index;
      };

      class CreateConstantBuffer : public RenderCommand
      {
      public:
        CreateConstantBuffer(CreateConstantBufferCommandDesc&& desc)
            : RenderCommand(rsl::move(desc.command))
            , m_desc(rsl::move(desc))
        {
        }

        ~CreateConstantBuffer() override = default;

        bool execute() override 
        {
          return backend::create_constant_buffer(cmd.create_constant_buffer_params, cmd.resource_slot);
        }

      private:
        CreateConstantBufferCommandDesc m_desc;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex