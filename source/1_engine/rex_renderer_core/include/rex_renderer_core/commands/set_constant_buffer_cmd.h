#pragma once

#include "rex_engine/types.h"

namespace rex
{
    namespace renderer
    {
        namespace commands
        {
            struct SetConstantBuffer
            {
                s32 buffer_index;
                s32 location;
                s32 offset;
            };
        }
    }
}