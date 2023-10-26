#pragma once

#include "rex_engine/types.h"

namespace rex
{
    namespace renderer
    {
        namespace commands
        {
            struct SetVertexBuffer
            {
                u32  buffer_index;
                u32  start_slot;
                u32* buffer_indices;
                u32  num_buffers;
                u32* strides;
                u32* offsets;
            };
        }
    }
}