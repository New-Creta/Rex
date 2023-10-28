#pragma once

#include "rex_engine/types.h"
#include "rex_renderer_core/clear_bits.h"
#include "rex_std_extra/math/color.h"

namespace rex
{
    namespace renderer
    {
        namespace parameters
        {
            struct ClearState
            {
                ClearState()
                    : rgba(0.0f, 0.0f, 0.0f, 1.0f)
                    , depth(1.0f)
                    , stencil(0x00)
                    , flags(ClearBits::CLEAR_COLOR_BUFFER)
                {}

                rsl::Color4f rgba;
                f32 depth;
                u8 stencil;
                ClearBits flags;
            };
        }
    }
}