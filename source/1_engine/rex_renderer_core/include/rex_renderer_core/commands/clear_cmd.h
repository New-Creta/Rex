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

                s32 clear_state;
                s32 array_index;
                s32 texture_index;
            };
        }
    }
}