#pragma once

#include "rex_engine/types.h"

namespace rex
{
    namespace renderer
    {
        namespace commands
        {
            struct SetRasterState
            {
                SetRasterState()
                    :raster_state(REX_INVALID_INDEX)
                {}

                s32 raster_state;
            };
        }
    }
}