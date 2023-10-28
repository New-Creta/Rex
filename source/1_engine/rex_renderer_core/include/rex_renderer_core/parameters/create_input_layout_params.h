#pragma once

#include "rex_engine/types.h"

#include "rex_renderer_core/vertex_buffer_format.h"

namespace rex
{
    namespace renderer
    {
        namespace parameters
        {
            struct InputLayoutDescription
            {
                const char8*        semantic_name;
                u32                 semantic_index;
                VertexBufferFormat  format;
                u32                 input_slot;
                u32                 aligned_byte_offset;
                u32                 input_slot_class;
                u32                 instance_data_step_rate;
            };

            struct CreateInputLayout
            {
                CreateInputLayout()
                    :input_layout(nullptr)
                    ,num_elements(0)
                {}

                InputLayoutDescription* input_layout;
                u32 num_elements;
            };
        }
    }
}