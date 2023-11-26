#pragma once

#include "rex_engine/types.h"

namespace rex
{
    struct DefaultTargetsInfo
    {
        s32 front_buffer_color = REX_INVALID_INDEX;
        s32 back_buffer_color = REX_INVALID_INDEX;
        s32 depth_buffer = REX_INVALID_INDEX;
    };
}