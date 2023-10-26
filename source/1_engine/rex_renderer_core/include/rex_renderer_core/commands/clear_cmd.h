#pragma once

#include "rex_engine/types.h"

namespace rex
{
    namespace renderer
    {
        namespace commands
        {
            struct Clear
            {
                Clear()
                    :clear_state(REX_INVALID_INDEX)
                    ,array_index(REX_INVALID_INDEX)
                    ,texture_index(REX_INVALID_INDEX)
                {}

                u32 clear_state;
                u32 array_index;
                u32 texture_index;
            };
        }
    }
}