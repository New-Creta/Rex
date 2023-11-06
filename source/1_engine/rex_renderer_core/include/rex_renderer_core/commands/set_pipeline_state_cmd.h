#pragma once

#include "rex_engine/types.h"

namespace rex
{
    namespace renderer
    {
        namespace commands
        {
            struct SetPipelineState
            {
                SetPipelineState()
                    :pipeline_state(REX_INVALID_INDEX)
                {}

                s32 pipeline_state;
            };
        }
    }
}