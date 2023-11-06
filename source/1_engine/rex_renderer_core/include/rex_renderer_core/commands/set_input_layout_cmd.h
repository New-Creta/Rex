#pragma once

#include "rex_engine/types.h"

namespace rex
{
    namespace renderer
    {
        namespace commands
        {
            struct SetInputLayout
            {
                SetInputLayout()
                    :input_layout(REX_INVALID_INDEX)
                {}

                s32 input_layout;
            };
        }
    }
}