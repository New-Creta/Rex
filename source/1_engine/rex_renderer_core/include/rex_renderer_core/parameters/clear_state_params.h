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
                ClearState()
                    :r(0.0f)
                    ,g(0.0f)
                    ,b(0.0f)
                    ,a(1.0f)
                    ,depth(1.0f)
                    ,stencil(0)
                    ,flags(0)
                {}

                f32 r, g, b, a;
                f32 depth;
                u8  stencil;
                u32 flags;
            };
        }
    }
}