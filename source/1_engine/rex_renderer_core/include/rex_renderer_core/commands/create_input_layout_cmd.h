#pragma once

#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_renderer_core/input_layout_classification.h"
#include "rex_renderer_core/vertex_buffer_format.h"

#include "rex_std/string_view.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct InputLayoutDescription
      {
        rsl::string_view semantic_name;
        s32 semantic_index;
        VertexBufferFormat format;
        s32 input_slot;
        s32 aligned_byte_offset;
        InputLayoutClassification input_slot_class;
        s32 instance_data_step_rate;
      };

      struct CreateInputLayoutCommandDesc
      {
        rsl::vector<InputLayoutDescription> input_layout;
      };

      class CreateInputLayout : public RenderCommand
      {
      public:
        CreateInputLayout(CreateInputLayoutCommandDesc&& desc, const ResourceSlot& slot)
            : RenderCommand()
            , m_desc(rsl::move(desc))
            , m_resource_slot(slot)
        {
        }

        ~CreateInputLayout() override = default;

        bool execute() override
        {
          return backend::create_input_layout(m_desc, m_resource_slot);
        }

      private:
        CreateInputLayoutCommandDesc m_desc;
        ResourceSlot m_resource_slot;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex