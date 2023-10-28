#pragma once

#include "rex_engine/types.h"

namespace rex
{
    namespace renderer
    {
        //-------------------------------------------------------------------------
        enum class ClearBits
        {
            NONE = REX_INVALID_INDEX,
            CLEAR_COLOR_BUFFER = BIT(0),
            CLEAR_DEPTH_BUFFER = BIT(1),
            CLEAR_STENCIL_BUFFER = BIT(2),
        };

        //-------------------------------------------------------------------------
        bool operator&(ClearBits bits1, ClearBits bits2)
        {
            return (static_cast<u32>(bits1) & static_cast<u32>(bits2)) != 0;
        }

        //-------------------------------------------------------------------------
        bool operator|(ClearBits bits1, ClearBits bits2)
        {
            return (static_cast<u32>(bits1) | static_cast<u32>(bits2)) != 0;
        }
    }
}