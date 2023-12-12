#pragma once

#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_renderer_core/input_layout_classification.h"
#include "rex_renderer_core/vertex_buffer_format.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct InputLayoutDescription
      {
        const char8* semantic_name;
        s32 semantic_index;
        VertexBufferFormat format;
        s32 input_slot;
        s32 aligned_byte_offset;
        InputLayoutClassification input_slot_class;
        s32 instance_data_step_rate;
      };

      struct CreateInputLayoutCommandDesc
      {
        RenderCommandDesc command;

        InputLayoutDescription* m_input_layout;
        s32 m_num_elements;
      };

      class CreateInputLayout : public RenderCommand
      {
      public:
        CreateInputLayout(CreateInputLayoutCommandDesc&& desc)
            : RenderCommand(rsl::move(desc.command))
            , m_desc(rsl::move(desc))
        {
        }

        ~CreateInputLayout() override = default;

        bool execute() override
        {
          result = backend::create_input_layout(cmd.create_input_layout_params, cmd.resource_slot);
          memory_free(cmd.create_input_layout_params.input_layout);

          return result;
        }

      private:
        CreateInputLayoutCommandDesc m_desc;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex