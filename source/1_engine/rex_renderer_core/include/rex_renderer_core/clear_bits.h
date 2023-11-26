#pragma once

#include "rex_engine/defines.h"
#include "rex_engine/types.h"

namespace rex
{
    namespace renderer
    {
        //-------------------------------------------------------------------------
        enum class ClearBits
        {
            NONE = BIT(0),
            CLEAR_COLOR_BUFFER = BIT(1),
            CLEAR_DEPTH_BUFFER = BIT(2),
            CLEAR_STENCIL_BUFFER = BIT(3),
        };

        //-------------------------------------------------------------------------
        REX_FORCE_INLINE bool operator&(ClearBits bits1, ClearBits bits2)
        {
            return (static_cast<u32>(bits1) & static_cast<u32>(bits2)) != 0;
        }

        //-------------------------------------------------------------------------
        REX_FORCE_INLINE bool operator|(ClearBits bits1, ClearBits bits2)
        {
            return (static_cast<u32>(bits1) | static_cast<u32>(bits2)) != 0;
        }
    }
}