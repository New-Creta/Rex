#pragma once

#include "rex_engine/types.h"

#include "rex_renderer_core/vertex_buffer_format.h"
#include "rex_renderer_core/input_layout_classification.h"

namespace rex
{
    namespace renderer
    {
        namespace parameters
        {
            struct InputLayoutDescription
            {
                const char8*                semantic_name;
                s32                         semantic_index;
                VertexBufferFormat          format;
                s32                         input_slot;
                s32                         aligned_byte_offset;
                InputLayoutClassification   input_slot_class;
                s32                         instance_data_step_rate;
            };

            struct CreateInputLayout
            {
                CreateInputLayout()
                    :input_layout(nullptr)
                    ,num_elements(0)
                {}

                InputLayoutDescription* input_layout;
                s32 num_elements;
            };
        }
    }
}