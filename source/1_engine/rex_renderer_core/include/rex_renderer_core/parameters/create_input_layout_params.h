#pragma once

#include "rex_engine/types.h"

namespace rex
{
    namespace renderer
    {
        namespace parameters
        {
            struct InputLayoutDescription
            {
                const char8*    semantic_name;
                u32             semantic_index;
                u32             format;
                u32             input_slot;
                u32             aligned_byte_offset;
                u32             input_slot_class;
                u32             instance_data_step_rate;
            };

            struct CreateInputLayout
            {
                CreateInputLayout()
                    :input_layout(nullptr)
                    ,num_elements(0)
                    ,vs_byte_code(nullptr)
                    ,vs_byte_code_size(0)
                {}

                InputLayoutDescription* input_layout;
                u32                     num_elements;
                void*                   vs_byte_code;
                u32                     vs_byte_code_size;
            };
        }
    }
}