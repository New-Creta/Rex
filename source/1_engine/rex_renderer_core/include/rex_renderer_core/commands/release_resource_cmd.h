#pragma once

#include "rex_engine/types.h"

namespace rex
{
    namespace renderer
    {
        namespace commands
        {
            struct ReleaseResource
            {
                ReleaseResource()
                    :resource_index(REX_INVALID_INDEX)
                {}

                u32 resource_index;
            };
        }
    }
}