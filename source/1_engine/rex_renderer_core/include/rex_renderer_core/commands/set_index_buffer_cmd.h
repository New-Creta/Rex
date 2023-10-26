#pragma once

#include "rex_engine/types.h"

namespace rex
{
    namespace renderer
    {
        namespace commands
        {
            struct SetIndexBuffer
            {
                SetIndexBuffer()
                    :buffer_index(REX_INVALID_INDEX)
                    ,format(0)
                    ,offset(0)
                {}

                u32 buffer_index;
                u32 format;
                u32 offset;
            };
        }
    }
}