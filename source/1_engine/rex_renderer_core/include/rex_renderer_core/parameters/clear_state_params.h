#pragma once

#include "rex_engine/types.h"

namespace rex
{
    namespace renderer
    {
        namespace parameters
        {
            struct ClearState
            {
                f32 r, g, b, a;
                f32 depth;
                u8  stencil;
                u32 flags;
            };
        }
    }
}