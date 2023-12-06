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
                s32 target;
                s32 location;
            };
        }
    }
}