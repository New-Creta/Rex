#pragma once

#include "rex_renderer_core/commands/render_cmd.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct CreateBufferCommandDesc
      {
        RenderCommandDesc command;

        s32 buffer_size;
        void* buffer_data;
      };

      class CreateBuffer : public RenderCommand
      {
      public:
        CreateBuffer(CreateBufferCommandDesc&& desc)
            : RenderCommand(rsl::move(desc.command))
            , m_desc(rsl::move(desc))
        {
        }

        ~CreateBuffer() override = default;

      protected:
        const CreateBufferCommandDesc& description() const { return m_desc; }

      private:
          CreateBufferCommandDesc m_desc;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex