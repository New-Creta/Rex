#pragma once

#include "rex_engine/types.h"
#include "rex_engine/defines.h"

#include "rex_engine/state_controller.h"

namespace rex
{
    namespace renderer
    {
        namespace parameters
        {
            struct CreateBuffer
            {
                CreateBuffer()
                    :buffer_size(0)
                    ,data(nullptr)
                {}

                s32 buffer_size;
                void* data;
            };
        }
    }
}