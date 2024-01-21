#pragma once

#include "rex_engine/engine/types.h"

namespace rex
{
    namespace renderer
    {
        struct Info
        {
            rsl::small_stack_string api_version;
            rsl::small_stack_string shader_version;
            rsl::small_stack_string adaptor;
            rsl::small_stack_string vendor;
        };
    }
}