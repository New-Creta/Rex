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
                s32  buffer_index;
                s32  start_slot;
                s32* buffer_indices;
                s32  num_buffers;
                s32* strides;
                s32* offsets;
            };
        }
    }
}